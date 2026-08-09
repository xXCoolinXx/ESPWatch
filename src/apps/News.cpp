#include "src/apps/News.h"
#include "src/kernel/Kernel.h"
#include "src/config/Secrets.h"
#include "src/config/GFXConstants.h"
#include <HTTPClient.h>
#include "ArduinoJson.h"

const String guardian = "https://content.guardianapis.com/search?show-blocks=body&tag=us-news/us-news&page-size=1&type=article";
const String hf_bart = "https://router.huggingface.co/hf-inference/models/facebook/bart-large-cnn";

String getNewsArticle(short idx=1) {
  HTTPClient http;

  String url = guardian + GUARDIAN_API;
  url += "&page=" + String(idx);
  
  http.begin(url.c_str());

  int http_code = http.GET();

  if(http_code > 0) {
    // Serial.println(http.getString());

    Serial.print("Guardian error: "); // <<< ADDED
    Serial.println(http_code);       // <<< ADDED

    JsonDocument doc; 
    DeserializationError error = deserializeJson(doc, http.getString());

    if(error) { http.end(); return ""; }

    // Serial.println(doc["response"]["results"][0]["webTitle"]);
    String title = doc["response"]["results"][0]["webTitle"];
    String content = doc["response"]["results"][0]["blocks"]["body"][0]["bodyTextSummary"];
    http.end();
    return  (title + ": " + content).substring(0, 1536); // Cap at 1.5 KB
  } else {
    Serial.println("Failed to read URL");
    http.end();
    return "";
  }
} 

String bartSummarize(String& news) {
  HTTPClient http;
  http.begin(hf_bart);
  
  String bearerAuth = "Bearer ";
  bearerAuth += HF_TOKEN;
  http.addHeader("Authorization", bearerAuth);

  // 2. Content-Type Header
  http.addHeader("Content-Type", "application/json");

  news.replace("\\", "\\\\"); // Keep escaping for good practice
  news.replace("\"", "\\\"");
  news.replace("\'", "\\\'");
  news.replace("\n", "\\n");
  news.replace("\r", "\\r");
  news.replace("\t", "\\t");
  news.replace("\b", "\\b");
  news.replace("\f", "\\f");

  String payload = "{ \"parameters\": {\"truncation\": \"longest_first\", \"max_length\": 100}, \"inputs\": \"";
  payload += news;
  payload += "\"}";
  int http_response = http.POST(payload);

  // Serial.println(payload);

  if(http_response > 0) {
    // Serial.println(http_response);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, http.getStream());

    if(error) {
      http.end();
      return "";
    } else {
      http.end();
      return doc[0]["summary_text"];
    }
  } else {
    http.end();
    return "";
  }
}

News::News(Kernel& kernel) : App(kernel) {
  // this->kernel.display.setTextColor(TFT_WHITE, TFT_BLACK, true);
  
  this->kernel.request_wifi();
  // Create display sprite
  this->kernel.display.unloadFont();
  this->news_sprite = std::make_unique<TFT_eSprite>(&(this->kernel.display));
  this->news_sprite->setColorDepth(1);
  if(this->news_sprite->createSprite(viewbox_wh, viewbox_wh) == nullptr) {
    Serial.println("News sprite creation failed");
    this->news_sprite.reset();
    return;
  }
  this->news_sprite->setTextSize(1);
  this->news_sprite->setTextFont(1); //
  this->news_sprite->setTextColor(TFT_WHITE, TFT_BLACK, true);
  this->news_sprite->setTextWrap(true);
}

void News::run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt) {
  // Slide left and right based on the active gesture
  if(gesture) {
    switch(gesture.value()) {
      case TouchHandler::Gesture::SWIPE_RIGHT:
        if(this->news_idx > 1) {
          this->news_idx -= 1;
          updateNews();
        }
        break;
      case TouchHandler::Gesture::SWIPE_LEFT:
        this->news_idx += 1;
        updateNews();
        break;
      default:
        // do nothing
        break;
    }
  }
  
  // Guards against empty news (I think)
  if(this->news_summary == "") {
    this->updateNews();
  }
  if(this->news_sprite) {
    this->news_sprite->pushSprite(left_vb, top_vb);
  }
}

String News::get_name() {
  return String("News");
}

void News::updateNews() {
  if(!this->news_sprite) {
    Serial.println("News sprite unavailable, skipping update");
    return;
  }

  this->kernel.clearViewBox();
  this->news_sprite->fillSprite(TFT_BLACK);
  this->news_sprite->setCursor(0, 0);
  this->news_sprite->print("Loading...");
  this->news_sprite->pushSprite(left_vb, top_vb);

  Serial.printf("Heap: %u\n", ESP.getFreeHeap());
  String content = getNewsArticle(this->news_idx);
  Serial.printf("Heap: %u\n", ESP.getFreeHeap());
  Serial.println(content);
  this->news_summary = bartSummarize(content);
  Serial.printf("Heap: %u\n", ESP.getFreeHeap());
  Serial.println(this->news_summary);

  this->kernel.clearViewBox();
  this->news_sprite->fillSprite(TFT_BLACK);
  this->news_sprite->setCursor(0, 0);
  this->news_sprite->print(news_summary);
}

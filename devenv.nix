{ pkgs, lib, config, inputs, ... }:

let

  defaultPort = "/dev/ttyACM0";
in
{
  # https://devenv.sh/packages/
  packages = with pkgs; [ 
    git 
    
    # Arduino tools
    arduino-cli
    esptool
    python3Packages.pyserial 
    picocom
    minicom
  ];

  # https://devenv.sh/languages/

  # https://devenv.sh/processes/
  # processes.dev.exec = "${lib.getExe pkgs.watchexec} -n -- ls -la";

  # https://devenv.sh/services/
  # services.postgres.enable = true;

  # https://devenv.sh/scripts/
  env = {
    ARDUINO_CONFIG_FILE = "./arduino-cli.yaml"; 

    ARDUINO_DIRECTORIES_DATA = "./.devenv/arduino/data";
    ARDUINO_DIRECTORIES_USER = "./.devenv/arduino/user";
    ARDUINO_DIRECTORIES_DOWNLOADS = "./.devenv/arduino/staging";
  };

  scripts = {
    # arduino-init.exec = ''
    #   arduino-cli core update-index
    #
    #   echo "Arduino configuration initialized"
    # ''; 
    #
    
    lsp-gen.exec = ''
      echo "📝 Generating compile_commands.json..."
      arduino-cli compile --only-compilation-database --build-path .devenv/build
      cp .devenv/build/compile_commands.json ./compile_commands.json 
    ''; # copy the compile_commands.json so that the LSP sees it :)

    compile-sketch.exec = ''
      echo "🔨 Compiling for ESP32-S3..."
      arduino-cli compile --build-path .devenv/build
      cp .devenv/build/compile_commands.json ./compile_commands.json
    '';

    upload-sketch.exec = ''
      PORT=''${PORT:-${defaultPort}}
      echo "⚡ Uploading to $PORT..."
      arduino-cli upload -p "$PORT" .
    '';

    flash-sketch.exec = ''
      PORT=''${PORT:-${defaultPort}}
      compile-sketch . && upload-sketch .
    '';

    monitor-serial.exec = ''
      PORT=''${PORT:-${defaultPort}}
      echo "📺 Opening serial monitor on $PORT at 115200 baud (Ctrl+A then Ctrl+X to exit)..."
      picocom "$PORT" -b 115200
    '';
  };

  # https://devenv.sh/basics/
  enterShell = ''
    git --version # Use packages
  '';

  # https://devenv.sh/tasks/
  # tasks = {
  #   "myproj:setup".exec = "mytool build";
  #   "devenv:enterShell".after = [ "myproj:setup" ];
  # };

  # https://devenv.sh/tests/
  enterTest = ''
    echo "Running tests"
    git --version | grep --color=auto "${pkgs.git.version}"
  '';

  # https://devenv.sh/git-hooks/
  # git-hooks.hooks.shellcheck.enable = true;

  # See full reference at https://devenv.sh/reference/options/
}

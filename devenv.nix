{ pkgs, lib, config, inputs, ... }:

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
    ARDUINO_BOARD_MANAGER_ADDITIONAL_URLS = "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json";
    
    ARDUINO_DIRECTORIES_DATA = "./.devenv/arduino/data";
    ARDUINO_DIRECTORIES_USER = "./.devenv/arduino/user";
    ARDUINO_DIRECTORIES_DOWNLOADS = "./.devenv/arduino/staging";
  };

  scripts.arduino-init.exec = ''
    arduino-cli config init --overwrite
    arduino-cli lib install "TFT_eSPI"
    arduino-cli lib install "CST816S"
    arduino-cli lib install "SensorLib"

    echo "Arduino configuration initialized"
  '';

  # https://devenv.sh/basics/
  enterShell = ''
    arduino-init
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

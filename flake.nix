{
  description = "ping pong game written in c++ with raylib library";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      name = "pingg-pongg";
      version = "1.0.0";
      pkgs = import nixpkgs {
        inherit system;
      };
    in
    {

      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = name;
        version = version;
        src = self;

        nativeBuildInputs = with pkgs; [
          gcc
          pkg-config
          autoPatchelfHook
        ];
        buildInputs = with pkgs; [
          libGL
          glfw3
          raylib
          raygui
        ];

        buildPhase = ''
          make -j8
        '';
        installPhase = ''
          mkdir -p $out/bin
          mv build/pingg-pongg $out/bin/
        '';

      };
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = [
          pkgs.gcc
          pkgs.clang-tools
          pkgs.pkg-config
          pkgs.gdb
          pkgs.gdbgui
          pkgs.valgrind
          pkgs.package-version-server
          pkgs.yaml-language-server
          pkgs.bash-language-server
        ];

        buildInputs = [
          pkgs.libGL
          pkgs.glfw3
          pkgs.raylib
          pkgs.raygui
        ];

        shellHook = ''
          export NIX_DEV_SHELL="${name}:${version}"
        '';
      };
    };
}

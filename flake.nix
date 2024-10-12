{
  description = "Next generation web engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?tag=24.05";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = { self, nixpkgs, nixgl, ... }:
    let
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];

      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      nixpkgsFor = forAllSystems (system:
        import nixpkgs {
          inherit system;
          overlays = [ nixgl.overlay ];
        }
      );
    in
    {
      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            packages = [
              pkgs.python312
              pkgs.gcc
              pkgs.pkg-config
              pkgs.clang
              pkgs.clang-tools
              pkgs.nixgl.auto.nixGLDefault

              # required by skia
              pkgs.libjpeg
              pkgs.zlib
              pkgs.fontconfig
              pkgs.expat
              pkgs.glfw
              pkgs.libGLU
              pkgs.mesa
              pkgs.xorg.libX11
              pkgs.libwebp
              pkgs.icu
            ];

            shellHook = "export CC=clang++";
          };
        }
      );
    };
}

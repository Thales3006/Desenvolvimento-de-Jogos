{
  description = "Dev environment for the SFML tilemap sandbox (sfml-tilemap-test branch)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        # `nix develop ./sandbox` drops you into a shell with SFML 3, CMake and
        # a C++ toolchain on the path. Because SFML is in buildInputs, its CMake
        # config is found automatically -- no manual CMAKE_PREFIX_PATH export.
        #
        #   nix develop ./sandbox
        #   cmake -S sandbox -B sandbox/build
        #   cmake --build sandbox/build
        #   ./sandbox/build/tilemap_sandbox
        #
        # (Run from the repo root so the sandbox can reach ../source.)
        devShells.default = pkgs.mkShell {
          # SFML 3: graphics / window / system, plus its transitive deps.
          buildInputs = [ pkgs.sfml ];

          nativeBuildInputs = [
            pkgs.cmake
            pkgs.pkg-config
            pkgs.gcc
          ];
        };
      });
}

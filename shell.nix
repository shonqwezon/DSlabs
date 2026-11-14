{ pkgs ? import <nixpkgs> {} }:

(pkgs.mkShell.override {
  stdenv = pkgs.gccStdenv;
}) {
  packages = [
    pkgs.gnumake
    pkgs.python3
  ];

  shellHook = ''
    export CC="${pkgs.gccStdenv.cc}/bin/gcc"
    export CXX="${pkgs.gccStdenv.cc}/bin/g++"

    if [ -t 0 ] && [ -t 1 ]; then
      exec zsh
    fi
  '';
}

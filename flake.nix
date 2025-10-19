{
  description = "WiiMix Development Shell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
      let
	system = "x86_64-linux"

        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.${system}.default = pkgs.mkShell {
          
	  # Tools needed to run build
	  nativeBuildInputs = [
	    pkgs.gcc
  	    pkgs.cmake
  	    pkgs.pkg-config
  	    pkgs.ninja	
 	  ];
	  
	  # Libraries linked against
	  buildInputs = [
	    pkgs.curl
	    pkgs.qt6.qtbase.dev
	    pkgs.dbus.dev
	    pkgs.libpqxx.dev
	  ];
        };
      }
}

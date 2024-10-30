{ pkgs }: {
	deps = [
   pkgs.inetutils
		pkgs.clang
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}
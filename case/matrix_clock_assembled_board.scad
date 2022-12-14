use </mattwach/util.scad>

module matrix_clock_assembled_board() {
  import_stl("matrix_clock_kicad_pcb.stl");
}

$fa=2;
$fs=0.2;
matrix_clock_assembled_board();

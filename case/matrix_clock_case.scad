use <mattwach/util.scad>
include <lib/matrix_clock_assembled_board.scad>

module matrix_clock_case() {
  matrix_clock_assembled_board();
}

$fa=2;
$fs=0.2;
matrix_clock_case();

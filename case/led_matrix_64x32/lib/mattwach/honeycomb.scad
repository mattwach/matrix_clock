// Implements a honeycomb grid

module honeycomb(size, side_width, gap) {
  xsize = size[0];
  ysize = size[1];
  zsize = size[2];
  span = side_width + gap;

  xsteps = floor(xsize / span);
  ysteps = floor(ysize / span);

  module hexagon() {
    r = side_width * 0.5 / cos(30);
    linear_extrude(zsize)
      polygon([ for (a = [0:60:360]) [cos(a) * r, sin(a) * r]]);
  }

  translate([
    -cos(30) * span * xsteps / 2,
    span * (-sin(30) * xsteps - ysteps) / 2,
    0
  ])
    for (x = [0:xsteps]) {
      translate([cos(30) * span * x, sin(30) * span * x, 0])
        for (y = [0:ysteps]) {
          translate([0, y * span, 0]) hexagon();
        }
    }
}

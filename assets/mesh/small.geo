Point(1) = {0.0,   -0.095, 0, 1.0};
Point(2) = {0.1,   -0.095, 0, 1.0};
Point(3) = {0.1,    0.005, 0, 1.0};
Point(4) = {0.0,    0.005, 0, 1.0};

Line(1) = {1, 2}; // bottom
Line(2) = {2, 3}; // right
Line(3) = {3, 4}; // top
Line(4) = {4, 1}; // left

Line Loop(1) = {1, 2, 3, 4};
Plane Surface(1) = {1};

Transfinite Line{1, 3} = 4; // along x
Transfinite Line{2, 4} = 4; // along y

Transfinite Surface{1} = {1, 2, 3, 4};
Recombine Surface{1};

Physical Surface("Domain") = {1};
Physical Line("Bottom") = {1};
Physical Line("Right")  = {2};
Physical Line("Top")    = {3};
Physical Line("Left")   = {4};

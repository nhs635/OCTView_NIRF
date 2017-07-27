function [result_image] = circularize(image)

DIAMETER = 1024;
[width, height] = size(image);

% Generate circularize map
[X, Y] = meshgrid(linspace(1, DIAMETER, DIAMETER), linspace(1, DIAMETER, DIAMETER));
X0 = DIAMETER / 2;
Y0 = DIAMETER / 2;
X = X - X0;
Y = Y - Y0;

% X map: interpolate (-pi, pi) -> (1, width)
theta = atan2(Y, X);
x_map = (theta + pi) * (width - 1) / (2 * pi) + 1;

% Y map: interpolate (0, RADIUS) -> (1, height)
rho = sqrt(X.^2 + Y.^2);
y_map = rho * (height - 1) / (DIAMETER/2) + 1;

% Do 2D interpolation
result_image = interp2(image, x_map, y_map);

% imshow(result_image);

end
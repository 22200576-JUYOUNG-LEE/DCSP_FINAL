function find_coff(Vin1, Vin2, Vin3, b1,b2,b3)
    v = [Vin1; Vin2; Vin3];
    b = [b1; b2; b3];

    A = [v.^2, v, ones(3,1)];
    x = A \ b;

    a = x(1);
    b = x(2);
    c = x(3);
    fprintf('a = %.6f\n', a);
    fprintf('b = %.6f\n', b);
    fprintf('c = %.6f\n', c);


end
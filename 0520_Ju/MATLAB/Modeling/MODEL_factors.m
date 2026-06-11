function factor = MODEL_factors(Gs)
    [num, den] = tfdata(Gs, 'v');
   
    % 분모의 계수 추출
    a1 = den(1);
    a2 = den(2);
    a3 = den(3);

    b = num(3);

    
    Wn = sqrt(a3/ a1);
    gain = b / Wn^2;
    zeta = a2/(Wn * 2);

    factor = [gain Wn zeta];
end
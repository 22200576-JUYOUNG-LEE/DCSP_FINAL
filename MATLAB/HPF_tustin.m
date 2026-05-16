function Hz_tustin = HPF_tustin()

    %---------------------------------------------------------
    % analog control system
    %---------------------------------------------------------
    Wc = 2 * pi * 90; %  
    [CT_HPF_NUM, CT_HPF_DEN] = butter(1, Wc, 'high', 's');
    Hs = tf(CT_HPF_NUM, CT_HPF_DEN); % CT transfer function
    
    %---------------------------------------------------------
    % digital control system
    %---------------------------------------------------------
    Ts = 0.005;
    Hz_tustin = c2d(Hs, Ts, 'tustin');

end
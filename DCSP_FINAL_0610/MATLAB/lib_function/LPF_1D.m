function X_LPF = LPF_1D(Time, X, Wc)

    time_length     = length(Time);
    time_uniform    = linspace(Time(1), Time(end), time_length);

    sampling_period                 = Time(2) - Time(1);
    [CT_LPF_NUM, CT_LPF_DEN]        = butter(2, Wc, 'low', 's');

    % analog
    Hs                              = tf(CT_LPF_NUM, CT_LPF_DEN); % CT transfer function
    
    % digital
    Hz_tustin                       = c2d(Hs, sampling_period, 'tustin');
    
    X_LPF                           = lsim(Hz_tustin, X, time_uniform);
end
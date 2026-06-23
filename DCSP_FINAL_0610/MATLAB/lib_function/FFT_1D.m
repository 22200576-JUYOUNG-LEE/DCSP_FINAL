function  [Freq, FFT_Mag]= FFT_1D(Time, X, Freq_sampling)

    time_length         = length(Time);
    Freq                = Freq_sampling * (0:floor(time_length/2)) / time_length;

    fft_result          = fft(X);

    fft_mag                = abs(fft_result / time_length);
    FFT_Mag                = fft_mag(1:floor(time_length/2)+1, :);
    FFT_Mag(2:end-1, :)    = 2 * FFT_Mag(2:end-1, :); % DC 성분 제외 2배 보정

    FFT_Mag = 20 * log10(FFT_Mag);
end
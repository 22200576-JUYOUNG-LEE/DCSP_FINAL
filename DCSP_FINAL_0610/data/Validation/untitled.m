


Dataset = readmatrix('Valid_pulse_20rad.out', 'NumHeaderLines', 1, 'FileType', 'text');

data_Time          = Dataset(:,1);
data_Time_length   = length(data_Time);
data_Time_uniform  = linspace(data_Time(1), data_Time(end), data_Time_length)';

data_Wcmd          = Dataset(:,2);
data_Wgyro         = Dataset(:,5);

Km          = 11.8676/ 12.6778     ;   % [rad/(sec*V)]
Time_const  = 1 / 12.6778            ;   % [sec]
Gm          = Km / (Time_const*s + 1); % Motor bandwidth 2.7723 [Hz]


simin_Wgyro        = [data_Time_uniform data_Wgyro];
simin_Wcmd         = [data_Time_uniform data_Wcmd];


figure(1); hold on;
plot(data_Time_uniform, data_Wcmd);
plot(data_Time_uniform, data_Wgyro);
hold off;
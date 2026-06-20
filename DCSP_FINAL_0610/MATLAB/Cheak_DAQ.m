% subject: Digital control and signal Processing
% title: Check DAQ

clc;
clear all;
close all;

%==========================================================================
% Read Dataset
%==========================================================================
file = '../../data/Check_DAQ/Checking_data.out';

% Labeling
fid         = fopen(file, 'r');
firstLine   = fgetl(fid);
fclose(fid);
Labels = strsplit(strtrim(firstLine));

% Read Dataset
Dataset = readmatrix(file, 'NumHeaderLines', 1, 'FileType', 'text');

%==========================================================================
% Ploting
%==========================================================================
figure(1); hold on;

plot(Dataset(:,1), Dataset(:,2), 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', Labels{2});
plot(Dataset(:,1), Dataset(:,3), 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', Labels{3});
plot(Dataset(:,1), Dataset(:,4), 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', Labels{4});
plot(Dataset(:,1), Dataset(:,5), 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', Labels{5});
plot(Dataset(:,1), Dataset(:,6), 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', Labels{6});


xlabel("Time [sec]",'FontWeight','bold');
ylabel("Values [V]",'FontWeight','bold');

title("Checking - DAQ", 'FontWeight', 'bold', 'Interpreter', 'none');

legend('Location','best');
grid on;

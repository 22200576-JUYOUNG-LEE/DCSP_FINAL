X = 1;
Y = 2;

filename = 'Static_Linear/Static_withdeadzone.out';
basePath = '../data/';
fullPath = fullfile(basePath, filename);

%1. Get label on first line
fid = fopen(fullPath, 'r');
firstLine = fgetl(fid);
fclose(fid);
Labels = strsplit(strtrim(firstLine));

%2. Get Dataset
Dataset =  readmatrix(fullPath, 'NumHeaderLines', 1, 'FileType','text');

%3. Plot Dataset
figure(); hold on;

colName = Labels{2};% at using array use {}

plot(Dataset(:,X), Dataset(:,Y), 'o-', 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', colName);
hold on;
plot(Dataset(:,X), 1178*Dataset(:,X), 'r-', 'LineWidth', 1.2, 'MarkerSize', 3, 'DisplayName', colName);

 %4. Labling
xlabel(Labels{X},'FontWeight','bold');
ylabel(Labels{Y},'FontWeight','bold');

hold off;

%5. 경로와 확장자를 제외한 파일 이름만 추출 ('../data/Static/Static.out' -> 'Static')
[~, pureFilename, ~] = fileparts(filename);

title(pureFilename, 'FontWeight', 'bold', 'Interpreter', 'none');

legend('Location','best');
grid on;

close all;
clear all;

figure
hold all;

fid = fopen('traces_proc.bin');
ntrazas = fread(fid, 1, 'uint32');
cantmuestras = fread(fid, 1, 'uint32');

for i=1:ntrazas
	traza = fread(fid,cantmuestras,'double');
	plot(traza);
end

title('Señales capturadas (luego de decimación)')
xlabel('Tiempo [muestras]') % x-axis label
ylabel('Amplitud')
xlim([0 cantmuestras])

set(gcf,'PaperUnits','inches','PaperPosition',[0 0 4 2])
print('captura','-dpng','-r300')

fclose(fid);
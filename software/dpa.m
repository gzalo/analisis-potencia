close all;
clear all;

fid = fopen('dpa.bin');

figure;
hold all;
for i=1:256
	datos{i} = fread(fid,1000,'double');
end

ml = zeros(256,1);

for j=1:256
	plot(datos{j});
	ml(j) = max(datos{j});	
end

fclose(fid);

[B,I] = sort(ml, 'descend');

plot(datos{I(1)}, 'g','LineWidth',3);
fprintf('Clave mas probable: 0x%x\n', I(1)-1);
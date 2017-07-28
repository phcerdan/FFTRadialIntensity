#!/usr/bin/env Rscript
parent_folder = '~/repository_local/tem-saxs/build-release/results_SAXSim_K_carrageenan_4_1881_pixel_reconstruction_1x_binned/'
filename_root = 'K_carrageenan_4_1881_pixel_reconstruction_1x_binned_Slice_';
filename_end = '.plot';
Nfiles = 95;
filename = paste(parent_folder,filename_root,1, filename_end, sep='')
sample =  read.table(filename, col.names=c("d", "I"), row.names=NULL);

header = scan(file(filename), what="character", nlines=3)
# HEADER format:
# #input_file=thefilename
# #Nx=x
# #Ny=y
# fname = unlist(strsplit(header[2], "="))[2];
Nx = as.numeric(unlist(strsplit(header[4], "="))[2]);
Ny = as.numeric(unlist(strsplit(header[6], "="))[2]);
filename_output = paste('sum_result_', filename_root, 'MEANOF_', Nfiles, 'SLICES', sep='')
filename_output_full = paste(parent_folder, filename_output,'.plot', sep='') 
cat(paste('# input_file=', filename_output,"\n", sep=''), file = filename_output_full)
cat(paste('# Nx=', Nx,"\n", sep=''), file = filename_output_full, append=TRUE)
cat(paste('# Ny=', Ny,"\n", sep=''), file = filename_output_full, append=TRUE)

for (i in 2:Nfiles)
{
    print(i)
    filename = paste(parent_folder,filename_root,i, filename_end, sep='')
    print(filename);
    data = read.table(filename, col.names=c("d", "I"), row.names=NULL);
    .list <- list(data[,2], sample[,2])
    suma = Reduce('+', .list)
    sample[,2] = suma
}

suma = suma/Nfiles
suma_matrix = matrix(c (c(0:(length(suma)-1)), suma), ncol=2)
write(t(suma_matrix),file=filename_output_full, ncolumns=2, append=TRUE)
print('/home/phc/repository_local/tem-saxs/build-release/results_SAXSim_K_carrageenan_4_1881_pixel_reconstruction_1x_binned')

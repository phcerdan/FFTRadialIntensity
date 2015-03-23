#!/usr/bin/env Rscript

filename = commandArgs(trailingOnly=TRUE)[1];
print(paste("filename :",filename));
data = read.table(filename, col.names=c("d", "I"), row.names=NULL);
# print(data[,"d"]);
header = scan(file(filename), what="character", nlines=3)
# HEADER format:
# #input_file=thefilename
# #Nx=x
# #Ny=y
fname = unlist(strsplit(header[2], "="))[2];
Nx = as.numeric(unlist(strsplit(header[4], "="))[2]);
Ny = as.numeric(unlist(strsplit(header[6], "="))[2]);
print(paste("Nx =", Nx," ", "Ny =", Ny))

nm_per_pixel = commandArgs(trailingOnly=TRUE)[2];
print(paste("nm_per_pixel =",nm_per_pixel));
nm_per_pixel = as.numeric(nm_per_pixel);

dx = nm_per_pixel;
dy = nm_per_pixel;
dfx = 1.0/(Nx*dx);
dfy = 1.0/(Ny*dy);
df = sqrt(dfx*dfx + dfy*dfy);

q = data[,"d"] * df;
I = data[,"I"];

pdfName = paste(filename, ".pdf", sep='');
pdf(file=pdfName)

plot(q, I, log=c("x", "y"));
title(fname)
axis(1, q, outer=TRUE, xpd=TRUE)
dev.off(); # To switch off plot.
print(paste("Output pdf file generated:", pdfName));

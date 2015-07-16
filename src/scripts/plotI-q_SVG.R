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
dfx = 1.0/(dx);
dfy = 1.0/(dy);
df = min(dfx, dfy);
# df = sqrt(dfx*dfx + dfy*dfy);
# Nsize = sqrt(Nx^2 + Ny^2)
# df = (1.0/Nsize) * (1.0/dx);
Nmin = min(Nx,Ny);
dmax = as.integer(Nmin/2)
q = data[,"d"] * df / Nmin;
I = data[,"I"];

library(tools)
# if(q[1] == 0) I[1]=I[2]*10 # for correct I plot limits
datf = data.frame(q,I);
# #ADD SAXS DATA
# fileSaxs = "/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/SAXS_1car200NaCl10A_1237_longMod.txt"
# dataS = read.table(fileSaxs, col.names=c("d", "I"), row.names=NULL, skip=1);
# qS = dataS[,"d"];
# IS = dataS[,"I"]*10^11;
# datafS = data.frame(qS,IS);

library("ggplot2")
library("scales")
motherdir = dirname(filename);
filenameNoExtension = basename(file_path_sans_ext(filename));
p <-ggplot()+
    theme_bw() +
    # remove minor ticks
    theme(panel.grid.minor = element_blank()) +
    geom_line(data = datf, aes(x=q, y=I)) +
    # geom_line(data = datafS, colour="blue", aes(x = qS, y = IS)) +
    labs(title=filenameNoExtension, x=" q $nm^{-1}$", y="I") +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(SCALED=FALSE)
p

#####tikzDevice
# library("tikzDevice")
# options(tikzDefaultEngine = 'luatex')
# # options(tikzDefaultEngine = 'pdftex')
# options(tikzLatex=paste(latexroot,"pdflatex", sep=''));
# options(tikzLualatex=paste(latexroot,"lualatex",sep=''));
# options(tikzXelatex=paste(latexroot,"xelatex", sep=''));
###############
svgdir_bool = dir.create(file.path(motherdir,"svg"));
filenameSvg = paste(filenameNoExtension, ".svg", sep='');
svgName = file.path(motherdir,"svg", filenameSvg);
print(svgName)
svg(filename=svgName, width=6,height=6,pointsize=10)
p
dev.off(); # To switch off plot.
# print(paste("Output pdf file generated:", pdfName));

#!/usr/bin/env Rscript

# filename = commandArgs(trailingOnly=TRUE)[1];
filename =  "./carrageenanNa851.plot"
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

# nm_per_pixel = commandArgs(trailingOnly=TRUE)[2];
nm_per_pixel = 0.86;
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
#ADD SAXS DATA
fileSaxsLong = "/home/phc/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_Na/1car300NaCl10A_1254_long.dat"
dSaxsLong = read.table(fileSaxsLong, col.names=c("d", "I", "err"), row.names=NULL, skip=2);
qSL = dSaxsLong[,"d"];
ISL = dSaxsLong[,"I"]*10^11;
dfSL = data.frame(qSL,ISL);
fileSaxsShort = "/home/phc/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_Na/1car300NaCl10A_1238_short.dat"
dSaxsShort = read.table(fileSaxsShort, col.names=c("d", "I", "err"), row.names=NULL, skip=2);
qSS = dSaxsShort[,"d"];
ISS = dSaxsShort[,"I"]*10^11;
dfSS = data.frame(qSS,ISS);

library("ggplot2")
library("scales")
motherdir = dirname(filename);
filenameNoExtension = basename(file_path_sans_ext(filename));
p <-ggplot()+
    theme_bw() +
    # remove minor ticks
    theme(panel.grid.minor = element_blank()) +
    geom_line(data = datf, aes(x=q, y=I)) +
    geom_line(data = dfSL, colour="blue", aes(x = qSL, y = ISL)) +
    geom_line(data = dfSS, colour="blue", aes(x = qSS, y = ISS)) +
    labs(title=filenameNoExtension, x=" q $nm^{-1}$", y="I") +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(SCALED=FALSE) +
#### DRAW SEGMENTS ### 
    # slope = log(yend/y)/log(xend/x)
    # geom_segment( aes(x=10^-2, y=10^7, xend=10^-0, yend=10^5)) +  # slope=-1
    # geom_text(aes(x=10^-1.4, y=10^4.5 ), label='a = -1', angle = atan2(-1,1) * 180/pi )  +
    geom_segment( aes(x=10^-2, y=10^14, xend=10^-1, yend=10^12))  # slope=-2
    # geom_text(aes(x=10^-0.4, y=10^4 ), label='a = -2', angle = atan2(-2,1) * 180/pi ) +
    # geom_segment( aes(x=10^-1, y=10^6, xend=10^0, yend=10^3))  # slope=-3
    # geom_text(aes(x=10^-0.4, y=10^5 ), label='a = -3', angle = atan2(-3,1) * 180/pi )
p

#####tikzDevice
library("tikzDevice")
# options(tikzDefaultEngine = 'luatex')
# # options(tikzDefaultEngine = 'pdftex')
# options(tikzLatex=paste(latexroot,"pdflatex", sep=''));
# options(tikzLualatex=paste(latexroot,"lualatex",sep=''));
# options(tikzXelatex=paste(latexroot,"xelatex", sep=''));
###############
tikzdir_bool = dir.create(file.path(motherdir,"tikz"));
filenameTex = paste(filenameNoExtension, ".tex", sep='');
tikzName = file.path(motherdir,"tikz", filenameTex);
print(tikzName)
tikz(file=tikzName, standAlone = TRUE)
p
dev.off(); # To switch off plot.
# print(paste("Output pdf file generated:", pdfName));

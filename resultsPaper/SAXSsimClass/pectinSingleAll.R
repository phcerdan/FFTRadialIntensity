#!/usr/bin/env Rscript

# filename = commandArgs(trailingOnly=TRUE)[1];
filenameRoot =  "./singleImagesFromPectin/"
dataframes <- list.files(path = filenameRoot, pattern = "p*", full.names=TRUE);
library(plyr)
list_df <- llply(dataframes, read.table, col.names=c("q", "I"), row.names=NULL);
filenameHeader =  "./singleImagesFromPectin/p0_0"
header = scan(file(filenameHeader), what="character", nlines=3)
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
Nmin = min(Nx,Ny);
dmax = as.integer(Nmin/2)
library(tools)
# df = sqrt(dfx*dfx + dfy*dfy);
# Nsize = sqrt(Nx^2 + Ny^2)
# df = (1.0/Nsize) * (1.0/dx);
list_df_mod = llply(seq_along(list_df), function(i) { a = data.frame( list_df[[i]]$q * df/Nmin, list_df[[i]]$I) ; names(a)=c("q","I"); a;} )
#remove first value:
list_df_mod_clean = llply(seq_along(list_df_mod), function(i) {list_df_mod[[i]][-1,]});
list_df_to_plot = llply(seq_along(list_df_mod_clean), function(i) { ddply(list_df_mod_clean[[i]],.(q,I), here(mutate), n=i) } )
alldata = do.call(rbind, list_df_to_plot);
IMean = aaply(laply(list_df_mod_clean, as.matrix), c(2,3), mean)[,"I"];
ISd = aaply(laply(list_df_mod_clean, as.matrix), c(2,3), sd)[,"I"];
IMeanDeviation = aaply(laply(list_df_mod_clean, as.matrix), c(2,3), mad)[,"I"];
IMin = aaply(laply(list_df_mod_clean, as.matrix), c(2,3), min)[,"I"];
IMax = aaply(laply(list_df_mod_clean, as.matrix), c(2,3), max)[,"I"];
dataMean = data.frame(list_df_mod_clean[[1]]$q,IMean);
names(dataMean) = c("q", "I");
# This shows a 95% Confidence Interval (CI): http://www.ncbi.nlm.nih.gov/pmc/articles/PMC2064100/
dataSd = data.frame(IMean - 1.96*ISd, IMean + ISd);
names(dataSd) = c("min", "max");
# eps(z)~ dz ~ d[log(y)]=1/ln(10) * dy/y
dataError = data.frame(IMin, IMax);
names(dataError) = c("min", "max");
dataMeanDeviation = data.frame(IMean - IMeanDeviation, IMean + IMeanDeviation);
names(dataMeanDeviation) = c("min", "max");
filenameO =  "./pectinHomogeneous.plot"
dataO = read.table(filenameO, col.names=c("d", "I"), row.names=NULL);
header = scan(file(filenameO), what="character", nlines=3)
fnameO = unlist(strsplit(header[2], "="))[2];
Nx = as.numeric(unlist(strsplit(header[4], "="))[2]);
Ny = as.numeric(unlist(strsplit(header[6], "="))[2]);
print(paste("Nx =", Nx," ", "Ny =", Ny))
Nmin = min(Nx,Ny);
qO = dataO[,"d"] * df / Nmin;
IO = dataO[,"I"];
datfO = data.frame(qO,IO);
datfO_trim = subset(datfO);
datfO_trim = datfO_trim[-1,]
# datfO_trim = subset(datfO, subset=datfO$q > 10^-2.1);
# if(q[1] == 0) I[1]=I[2]*10 # for correct I plot limits
#ADD SAXS DATA
# fileSaxs = "/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/SAXS_acid_gel_good.txt"
# dataS = read.table(fileSaxs, col.names=c("d", "I"), row.names=NULL, skip=0);
fileSaxs = "/home/phc/Dropbox/Shared-Geelong-Palmerston/pectin/Pectin1_acid/Acid_gel_Aus_data.txt"
dataS = read.table(fileSaxs, col.names=c("d", "I"), row.names=NULL, skip=1);
qS = dataS[,"d"];
IS = dataS[,"I"];
#Remove first value of Aus Data
qS = tail(qS,-1);
IS = tail(IS,-1);
dmerged = data.frame(qS,IS);
names(dmerged) = c("q", "I");
dmerged$I = dmerged$I * 10^12.7;
dmerged$q = dmerged$q * 10; # Change from A to nm
# Chop large q data:
# dmerged = subset(dmerged, subset=dmerged$q < 10^+0.2);
library("ggplot2")
library("scales")
motherdir = dirname(filename);
filenameNoExtension = basename(file_path_sans_ext(filename));
eps = 0.015;
# qbad = 10^-0.92;
qbad = 10^-1.33;
dbad = subset(dmerged, subset= dmerged$q < qbad + eps * qbad & dmerged$q > qbad - eps * qbad);
p <-ggplot()+
    theme_bw() +
    theme(panel.grid.minor = element_blank()) +
    # geom_ribbon(data = dataMean, aes(x=dataMean$q, y = dataMean$I , ymin=dataError$min, ymax= dataError$max), colour="gray87", fill="gray88", alpha=0.9 ) + # position=position_dodge(0.2))+
    # geom_errorbar(data = dataMean, aes(x=dataMean$q, y = dataMean$I , ymin=dataSd$min, ymax= dataSd$max), alpha=0.9, colour="gray78", width=0.02 ) + # position=position_dodge(0.2))+
    geom_errorbar(data = dataMean, aes(x=dataMean$q, y = dataMean$I , ymin=dataMeanDeviation$min, ymax= dataMeanDeviation$max, shape="2 SD"),show_guide = TRUE, alpha=0.3, colour="gray20", width=0.02 ) + # position=position_dodge(0.2))+
    # geom_point(data = alldata, aes(x=alldata$q, y=alldata$I, colour=n), size=0.1) +
    geom_line(data = alldata, aes(x=alldata$q, y=alldata$I, group=alldata$n, colour=n), alpha=0.2) +
    geom_line(data = dataMean, aes(x=dataMean$q, y = dataMean$I), size=0.8, colour = "red") +
    scale_colour_continuous(name="Image:") +
    scale_shape_discrete(name="Error-Bars") +
    guides(linetype=FALSE) +
    # geom_line(data = datf, aes(x=q, y=I)) +
    # geom_point(data = datf_trim, aes(x=datf_trim$q, y=datf_trim$I, colour="red"), size=1) +
    # geom_point(data = datf2_trim, aes(x=datf2_trim$q, y=datf2_trim$I*0.9), size=1) +
    geom_point(data = datfO_trim, aes(x=datfO_trim$q, y=datfO_trim$I*0.04), size=1) +
    # geom_point(data = datfO, aes(x=datfO$q, y=datfO$I*0.9), size=1) +
    # geom_line(data = dmerged, colour="blue", aes(x = dmerged$q, y = dmerged$I)) +
    labs(title="Single Image Analysis: Pectin Acid", x=" q $nm^{-1}$", y="I") +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(SCALED=FALSE)
#### DRAW SEGMENTS ###
    # slope = log(yend/y)/log(xend/x)
    # geom_segment( aes(x=10^-2, y=10^7, xend=10^-0, yend=10^5)) +  # slope=-1
    # geom_text(aes(x=10^-1.4, y=10^4.5 ), label='a = -1', angle = atan2(-1,1) * 180/pi )  +
# p  <- p + geom_segment( aes(x=10^-2, y=10^12, xend=10^-1, yend=10^10))  # slope=-2
    # geom_text(aes(x=10^-0.4, y=10^4 ), label='a = -2', angle = atan2(-2,1) * 180/pi ) +
    # geom_segment( aes(x=10^-1, y=10^6, xend=10^0, yend=10^3))  # slope=-3
    # geom_text(aes(x=10^-0.4, y=10^5 ), label='a = -3', angle = atan2(-3,1) * 180/pi )
## Region of interest ##
# p + geom_segment(aes(x = dbad$q, y = 0, xend = dbad$q, yend = dbad$I), colour="#CC6666");
p <- p +
    geom_vline(show_guide=F, aes(linetype="CutOff", xintercept=dbad$q), data = dbad,  colour="#CC6666") +
    geom_rect(aes(xmin = dbad$q, xmax = Inf, ymin = 0, ymax = Inf), fill="#CC6666", alpha=0.15)
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
tikz(file=tikzName, standAlone = TRUE, sanitize =FALSE)
p
dev.off(); # To switch off plot.
# print(paste("Output pdf file generated:", pdfName));


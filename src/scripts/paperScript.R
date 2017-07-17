source('readIQdata.R')
rootDirectory <- '/home/phc/repository_local/FFT-from-image-compute-radial-intensity/build-release/resultsDenoised';
# CARRAGEENAN
filenameOriginal <- file.path(rootDirectory, 'montage_832_negated_original.plot');
filenameBLSGSM5 <- file.path(rootDirectory,'blsgsm_5.plot');
filenameBLSGSM50 <- file.path(rootDirectory,'blsgsm_50.plot');
filenameBLSGSM05 <- file.path(rootDirectory,'blsgsm_05.plot');
filenameTV <- file.path(rootDirectory,'tv_negated.plot');
files = c(filenameOriginal, filenameBLSGSM5, filenameBLSGSM50, filenameBLSGSM05, filenameTV);
fnames = c("Original", "BLSGSM_5", "BLSGSM_50", "BLSGSM_05", "TV_001")
N <- length(files);
nm_per_pixel_carrK <- 0.86;

dataImg<- do.call(cbind, lapply(1:N, function(i) readIQdata(files[i], nm_per_pixel_carrK, fnames[i])))
keep <- names(dataImg)!="q";
keep[1] <- TRUE
dataImg <- dataImg[keep]

#ADD SAXS DATA
source ('mergeSaxsData.R')
fileSaxsLong = "/home/phc/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_K/1car30KCl10A_1259_long.dat"
fileSaxsShort = "/home/phc/Dropbox/Shared-Geelong-Palmerston/Carrageenan/Carrageenan_K/1car30KCl10A_1243_short.dat"
dataSaxs = mergeSaxs(fileSaxsLong, fileSaxsShort, 5);
dataSaxs$I = dataSaxs$I * 10^7.3;
dataSaxs$q = dataSaxs$q * 10; # Change from A to nm
# Chop large q data:
# dataSaxs = subset(dataSaxs, subset=dataSaxs$q < 10^+0.2);

library("ggplot2")
library("scales")
p <-ggplot(data=dataImg, aes(x=q)) +
    theme_bw() +
    # remove minor ticks
    theme(panel.grid.minor = element_blank()) +
    # geom_line(data = dataImg, aes(x=q, y=I)) +
    geom_line(aes(y=Original), colour = "brown") +
    geom_line(aes(y=BLSGSM_5), colour = "red") +
    geom_line(aes(y=BLSGSM_50), colour = "green") +
    geom_line(aes(y=BLSGSM_05), colour = "black") +
    geom_line(aes(y=TV_001), colour = "pink") +
    geom_line(data = dataSaxs, colour="blue", aes(x = q, y = I)) +
    labs(title="CarrageenanK 832", x=" q $nm^{-1}$", y="I") +
    scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x, n=4),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x, n=3),
                     labels = trans_format("log10", math_format(10^.x))
                     ) +
    annotation_logticks(scaled = FALSE)
print(p)

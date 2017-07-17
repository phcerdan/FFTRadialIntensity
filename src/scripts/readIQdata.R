readIQdata <- function(filename, nm_per_pixel, cName = "I") {
  #Read data.
  data = read.table(filename, col.names=c("q", "I"), row.names=NULL);
  #Read header.
  header = scan(file(filename), what="character", nlines=3)
  fname = unlist(strsplit(header[2], "="))[2];
  Nx = as.numeric(unlist(strsplit(header[4], "="))[2]);
  Ny = as.numeric(unlist(strsplit(header[6], "="))[2]);
  cat(paste("Nx =", Nx," ", "Ny =", Ny, "\n"))
  cat(paste("cName", cName, "\n"))

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
  q = data[,"q"] * df / Nmin;
  I = data[,"I"];
  # I[0]=I[1] # for correct I plot limits
  dataframe= data.frame(q,I);
  colnames(dataframe) <- c("q",cName);
  return (dataframe)
}

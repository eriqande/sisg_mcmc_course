

easyMCMC = function(niter, startval, proposalsd){
x = rep(0,niter)
x[1] = startval     
for(i in 2:niter){
currentx = x[i-1]
proposedx = rnorm(1,mean=currentx,sd=proposalsd) 
A = target(proposedx)/target(currentx)
if(runif(1)<A){
x[i] = proposedx       # accept move with probabily min(1,A)
} else {
x[i] = currentx        # otherwise "reject" move, and stay where we are
}
}
return(x)
}


target = function(x){
return((x>0 & x <1) + (x>2 & x<3))
}


pdf("target.pdf")
x= seq(-2,5,length=100)
plot(x,target(x),type="s")
dev.off()

pdf("goodtrace1.pdf")
z1 = easyMCMC(1000,0.5,1)
par(mfcol=c(2,1))
plot(z1,ylim=c(-2,5),type="l",main="Trace from starting point 0.5, sd=1")
hist(z1,xlim=c(-2,5))
dev.off()

pdf("goodtrace2.pdf")
z2 = easyMCMC(1000,2.5,1)
par(mfcol=c(2,1))
plot(z2,ylim=c(-2,5),type="l",main="Trace from starting point 2.5, sd=1")
hist(z2,xlim=c(-2,5))
dev.off()

pdf("goodtrace3.pdf")
par(mfcol=c(2,1))
plot(z1,ylim=c(-2,5),type="l",main="Traces from different starting points overlaid")
lines(z2,col=2)
dev.off()


pdf("badtrace1.pdf")
z3 = easyMCMC(1000,0.5,0.1)
par(mfcol=c(2,1))
plot(z3,ylim=c(-2,5),type="l",main="Trace from starting point 0.5, sd=0.1")
hist(z3,xlim=c(-2,5))
dev.off()

pdf("badtrace2.pdf")
z4 = easyMCMC(1000,2.5,0.1)
par(mfcol=c(2,1))
plot(z4,ylim=c(-2,5),type="l",main="Trace from starting point 2.5, sd=0.1")
hist(z4,xlim=c(-2,5))
dev.off()

pdf("badtrace3.pdf")
par(mfcol=c(2,1))
plot(z3,ylim=c(-2,5),type="l",main="Traces from different starting points overlaid")
lines(z4,col=2)
dev.off()

pdf("badtrace.pdf")
z1 = easyMCMC(1000,0.5,0.1)
z2= easyMCMC(1000,2.5,0.1)
plot(z,ylim=c(-2,5),type="l",main="Traces from starting points 0.5 and 2.5, sd=0.1")
lines(z2,col=2)
dev.off()

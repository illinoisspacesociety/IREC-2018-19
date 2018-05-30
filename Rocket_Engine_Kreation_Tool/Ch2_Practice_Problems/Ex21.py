# This is the page where I do practice problems to get familiar with
# actually putting problems and equations into codeself.
#---------------------------------------------------------------------------------#
# EX 2-1
from sympy import Symbol, Eq, solve

mi = 200;
mf = 130;
mstruct = 110;
burnTime = 3.0;
Isp = 240;
g0 = 9.8066;
global mProp;
global mFuel;
global c;
global It;

def mFuel(mf = mf,mi = mi):
    global mFuel;
    mFuel = mi-mf;
    return(mFuel);
print("mFuel is " + str(mFuel()))

def mProp(mf = mf, mstruct = mstruct):
    global mProp;
    mProp = mf - mstruct;
    return(mProp);
print("mProp is " + str(mProp()))

def MassRatio(mf = mf,mi = mi):
    MR = mf/mi;
    return(MR);
print("Mass Ratio is " + str(MassRatio()))

def PMR(mProp = mProp, mFuel = mFuel):
    PMR = mFuel/(mProp + mFuel);
    return(PMR);
print("Propellant Mass Ratio is " + str(PMR()))

def It(Isp = Isp,mFuel = mFuel):
    global It
    It = Isp*mFuel*g0
    return(It)
print("Total Impulse is " + str(It()))

wdot = mFuel*g0/burnTime
F = Isp*wdot
print(F)

w = mFuel*g0

c = Isp*g0
print(c)

aMax = F/mf
gMax = aMax/g0
print(gMax)

It = Symbol("It")
eqn1 = Eq(It/w,Isp)
print(solve(eqn1))

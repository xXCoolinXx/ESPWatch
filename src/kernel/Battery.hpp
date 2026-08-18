// Short header-only implementation of battery voltage to percentage. AI-generated
#include <cmath>
#include <algorithm>

/**
 * Calculate LiPo state-of-charge percentage using a logarithmic OCV model.
 *
 * @param V      Measured cell voltage (V).
 * @param vMax   Full charge voltage (V). Default: 4.20
 * @param vNom   Nominal voltage (V). Default: 3.70
 * @param vCut   Cutoff voltage (V). Default: 2.75
 * @param zMax   SOC at vMax (0-1). Default: 0.99
 * @param zNom   SOC at vNom (0-1). Default: 0.50
 * @param zCut   SOC at vCut (0-1). Default: 0.01
 * @param iters  Bisection solver iterations. Default: 60
 * @return       Battery charge percentage [0, 100].
 */
double stateOfChargePercent(double V, 
                           double vMax = 4.20, double vNom = 3.70, double vCut = 2.75,
                           double zMax = 0.99, double zNom = 0.50, double zCut = 0.01,
                           int iters = 60) {
    using namespace std;
    
    double lzF = log(zMax), lzN = log(zNom), lzE = log(zCut);
    double l1F = log(1.0 - zMax), l1N = log(1.0 - zNom), l1E = log(1.0 - zCut);
    
    double dB1 = lzF - lzN, dC1 = l1F - l1N, dV1 = vMax - vNom;
    double dB2 = lzN - lzE, dC2 = l1N - l1E, dV2 = vNom - vCut;
    
    double det = dB1 * dC2 - dB2 * dC1;
    double b = (dV1 * dC2 - dV2 * dC1) / det;
    double c = (dB1 * dV2 - dB2 * dV1) / det;
    double a = vNom - b * lzN - c * l1N;
    
    V = clamp(V, min(vMax, vCut), max(vMax, vCut));
    
    double lo = 1e-9, hi = 1.0 - 1e-9;
    for (int i = 0; i < iters; ++i) {
        double mid = 0.5 * (lo + hi);
        double vMid = a + b * log(mid) + c * log(1.0 - mid);
        if (vMid < V) lo = mid; else hi = mid;
    }
    
    return 100.0 * 0.5 * (lo + hi);
}

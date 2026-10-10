#pragma once

#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>
#include <limits>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Mathematical utilities for neural computations
 * Provides efficient implementations of common mathematical operations
 */
class MathUtils {
public:
    // Activation functions
    static MembranePotential sigmoid(MembranePotential x);
    static MembranePotential tanh(MembranePotential x);
    static MembranePotential relu(MembranePotential x);
    static MembranePotential leakyRelu(MembranePotential x, MembranePotential alpha = 0.01f);
    static MembranePotential elu(MembranePotential x, MembranePotential alpha = 1.0f);
    static MembranePotential selu(MembranePotential x, MembranePotential alpha = 1.673263, MembranePotential scale = 1.050701);
    static MembranePotential softplus(MembranePotential x);
    static MembranePotential softsign(MembranePotential x);
    static MembranePotential swish(MembranePotential x);
    static MembranePotential mish(MembranePotential x);
    
    // Derivative of activation functions
    static MembranePotential sigmoidDerivative(MembranePotential x);
    static MembranePotential tanhDerivative(MembranePotential x);
    static MembranePotential reluDerivative(MembranePotential x);
    static MembranePotential leakyReluDerivative(MembranePotential x, MembranePotential alpha = 0.01f);
    static MembranePotential eluDerivative(MembranePotential x, MembranePotential alpha = 1.0f);
    static MembranePotential seluDerivative(MembranePotential x, MembranePotential alpha = 1.673263, MembranePotential scale = 1.050701);
    static MembranePotential softplusDerivative(MembranePotential x);
    static MembranePotential softsignDerivative(MembranePotential x);
    static MembranePotential swishDerivative(MembranePotential x);
    static MembranePotential mishDerivative(MembranePotential x);
    
    // Statistical functions
    static MembranePotential mean(const std::vector<MembranePotential>& values);
    static MembranePotential variance(const std::vector<MembranePotential>& values);
    static MembranePotential standardDeviation(const std::vector<MembranePotential>& values);
    static MembranePotential median(const std::vector<MembranePotential>& values);
    static MembranePotential quantile(const std::vector<MembranePotential>& values, double q);
    static std::vector<MembranePotential> percentile(const std::vector<MembranePotential>& values, const std::vector<double>& quantiles);
    static MembranePotential correlation(const std::vector<MembranePotential>& x, const std::vector<MembranePotential>& y);
    static MembranePotential covariance(const std::vector<MembranePotential>& x, const std::vector<MembranePotential>& y);
    
    // Probability functions
    static MembranePotential pdfNormal(MembranePotential x, MembranePotential mean, MembranePotential stddev);
    static MembranePotential cdfNormal(MembranePotential x, MembranePotential mean, MembranePotential stddev);
    static MembranePotential pdfExponential(MembranePotential x, MembranePotential lambda);
    static MembranePotential cdfExponential(MembranePotential x, MembranePotential lambda);
    static MembranePotential pdfGamma(MembranePotential x, MembranePotential shape, MembranePotential scale);
    static MembranePotential cdfGamma(MembranePotential x, MembranePotential shape, MembranePotential scale);
    static MembranePotential pdfBeta(MembranePotential x, MembranePotential alpha, MembranePotential beta);
    static MembranePotential cdfBeta(MembranePotential x, MembranePotential alpha, MembranePotential beta);
    static MembranePotential pdfChiSquare(MembranePotential x, MembranePotential degreesOfFreedom);
    static MembranePotential cdfChiSquare(MembranePotential x, MembranePotential degreesOfFreedom);
    static MembranePotential pdfStudentT(MembranePotential x, MembranePotential degreesOfFreedom);
    static MembranePotential cdfStudentT(MembranePotential x, MembranePotential degreesOfFreedom);
    static MembranePotential pdfF(MembranePotential x, MembranePotential df1, MembranePotential df2);
    static MembranePotential cdfF(MembranePotential x, MembranePotential df1, MembranePotential df2);
    
    // Special functions
    static MembranePotential factorial(int n);
    static MembranePotential gamma(MembranePotential x);
    static MembranePotential lnGamma(MembranePotential x);
    static MembranePotential digamma(MembranePotential x);
    static MembranePotential trigamma(MembranePotential x);
    static MembranePotential beta(MembranePotential x, MembranePotential y);
    static MembranePotential lnBeta(MembranePotential x, MembranePotential y);
    
    // Vector operations
    static std::vector<MembranePotential> add(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static std::vector<MembranePotential> subtract(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static std::vector<MembranePotential> multiply(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static std::vector<MembranePotential> divide(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static MembranePotential dotProduct(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static std::vector<MembranePotential> crossProduct(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static MembranePotential norm(const std::vector<MembranePotential>& v);
    static MembranePotential distance(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    
    // Matrix operations
    static std::vector<std::vector<MembranePotential>> add(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b);
    static std::vector<std::vector<MembranePotential>> subtract(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b);
    static std::vector<std::vector<MembranePotential>> multiply(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b);
    static std::vector<std::vector<MembranePotential>> transpose(const std::vector<std::vector<MembranePotential>>& matrix);
    static MembranePotential matrixDeterminant(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> matrixInverse(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> matrixMultiply(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b);
    
    // Linear algebra
    static MembranePotential trace(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> matrixEigenvalues(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> matrixEigenvectors(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> qrDecomposition(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> luDecomposition(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> choleskyDecomposition(const std::vector<std::vector<MembranePotential>>& matrix);
    static std::vector<std::vector<MembranePotential>> svd(const std::vector<std::vector<MembranePotential>>& matrix);
    
    // Differential equations
    static MembranePotential forwardEuler(MembranePotential y, MembranePotential dy, MembranePotential dt);
    static MembranePotential backwardEuler(MembranePotential y, MembranePotential dy, MembranePotential dt);
    static MembranePotential rungeKutta4(MembranePotential y, std::function<MembranePotential(MembranePotential)> f, MembranePotential dt);
    static MembranePotential rungeKutta4(MembranePotential y, MembranePotential dydt, std::function<MembranePotential(MembranePotential, MembranePotential)> f, MembranePotential dt);
    
    // Optimization
    static std::pair<std::vector<MembranePotential>, MembranePotential> gradientDescent(const std::vector<MembranePotential>& x, const std::function<MembranePotential(const std::vector<MembranePotential>&)>& f, double learningRate, int maxIter, double tolerance);
    static std::pair<std::vector<MembranePotential>, MembranePotential> stochasticGradientDescent(const std::vector<MembranePotential>& x, const std::function<MembranePotential(const std::vector<MembranePotential>&)>& f, double learningRate, int maxIter, double tolerance);
    static std::pair<std::vector<MembranePotential>, MembranePotential> momentumOptimization(const std::vector<MembranePotential>& x, const std::function<MembranePotential(const std::vector<MembranePotential>&)>& f, double learningRate, double momentum, int maxIter, double tolerance);
    
    // Random sampling
    static std::vector<MembranePotential> randomSample(const std::vector<MembranePotential>& population, int sampleSize, bool withReplacement = false);
    static std::vector<MembranePotential> bootstrap(const std::vector<MembranePotential>& data, int sampleSize = -1);
    static std::vector<MembranePotential> jackknife(const std::vector<MembranePotential>& data, int leaveOneOut);
    static std::vector<MembranePotential> crossValidation(const std::vector<MembranePotential>& data, int folds);
    
    // Statistical tests
    static double tTest(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static double chiSquareTest(const std::vector<MembranePotential>& observed, const std::vector<MembranePotential>& expected);
    static double fTest(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b);
    static double anova(const std::vector<std::vector<MembranePotential>>& groups);
    
    // Data processing
    static std::string toString(MembranePotential value, int precision = 6);
    static std::vector<MembranePotential> toVector(const std::string& str, char delimiter = ',');
    static std::string fromVector(const std::vector<MembranePotential>& values, char delimiter = ',');
    static MembranePotential normalize(const std::vector<MembranePotential>& values, MembranePotential min = std::numeric_limits<MembranePotential>::lowest(), MembranePotential max = std::numeric_limits<MembranePotential>::max());
    static MembranePotential scale(const std::vector<MembranePotential>& values, MembranePotential mean = 0.0f, MembranePotential stddev = 1.0f);
    static MembranePotential standardize(const std::vector<MembranePotential>& values);
    
    // String operations
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, char delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    
    // Time operations
    static std::string formatTime(double seconds);
    static std::string formatDate(const std::tm& time);
    static std::tm parseDate(const std::string& dateStr);
    static double currentTimeMs();
    static double currentTimeUs();
    
    // Color conversions
    static std::string toHexColor(MembranePotential r, MembranePotential g, MembranePotential b, MembranePotential a = 1.0f);
    static std::string toRgbColor(MembranePotential r, MembranePotential g, MembranePotential b);
    static void fromHexColor(const std::string& hex, MembranePotential& r, MembranePotential& g, MembranePotential& b, MembranePotential& a);
    
    // Text formatting
    static std::string leftPad(const std::string& str, size_t width, char pad = ' ');
    static std::string rightPad(const std::string& str, size_t width, char pad = ' ');
    static std::string center(const std::string& str, size_t width, char pad = ' ');
    static std::string ellipsis(const std::string& str, size_t maxLength);
    
    // File operations
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern = "*");
    static std::vector<std::string> listDirectories(const std::string& directory);
    static bool createDirectory(const std::string& path);
    static bool deleteFile(const std::string& path);
    static bool deleteDirectory(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string removeFileExtension(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string absolutePath(const std::string& path);
    static std::string relativePath(const std::string& path, const std::string& base = ".");
    
    // Network operations
    static bool ping(const std::string& host);
    static std::string getHostName();
    static std::string getIPAddress(const std::string& hostName = "");
    static std::vector<std::string> resolveDNS(const std::string& hostName);
    
    // Process operations
    static std::vector<std::string> listProcesses();
    static std::string getProcessName(int pid);
    static bool killProcess(int pid);
    static bool isProcessRunning(int pid);
    static std::string getProcessCommandLine(int pid);
    
    // Environment variables
    static std::string getEnvironmentVariable(const std::string& name);
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    static bool removeEnvironmentVariable(const std::string& name);
    static std::vector<std::string> listEnvironmentVariables();
    
    // System information
    static std::string getOperatingSystem();
    static std::string getArchitecture();
    static std::string getProcessorName();
    static double getCpuUsage();
    static double getMemoryUsage();
    static double getDiskUsage(const std::string& path = "/");
    static std::vector<std::string> getRunningThreads();
    
    // Random numbers
    static double randomUniform(double min = 0.0, double max = 1.0);
    static double randomNormal(double mean = 0.0, double stddev = 1.0);
    static double randomExponential(double lambda = 1.0);
    static double randomGamma(double shape, double scale = 1.0);
    static double randomBeta(double alpha, double beta);
    static double randomChiSquare(double degreesOfFreedom);
    static double randomStudentT(double degreesOfFreedom);
    static double randomF(double df1, double df2);
    static double randomCauchy(double location = 0.0, double scale = 1.0);
    static double randomLaplace(double mu = 0.0, double b = 1.0);
    static double randomLogistic(double mu = 0.0, double s = 1.0);
    static double randomWeibull(double shape = 1.0, double scale = 1.0);
    static double randomUniformInt(int min, int max);
    static double randomBinomial(int n, double p);
    static double randomPoisson(double lambda);
    static double randomHypergeometric(int N, int K, int n);
    static double randomGeometric(double p);
    static double randomPascal(int r, double p);
    static double randomNegativeBinomial(double p, int r);
    static double randomPoissonBinomial(const std::vector<double>& probabilities);
    static double randomWrightFisher(int N, double p);
    static double randomMoran(int N, double p);
    static double randomEwens(double theta, int n);
    static double randomBetaBinomial(int n, double alpha, double beta);
    static double randomBetaNegativeBinomial(int r, double alpha, double beta);
    static double randomGammaBinomial(int n, double alpha, double beta);
    static double randomBinomialBeta(double p, double alpha, double beta);
    static double randomMixture(const std::vector<double>& weights,
                               const std::vector<std::function<double()>>& generators,
                               const std::vector<std::vector<double>>& params);
    static double randomMixtureOfNormals(const std::vector<double>& weights,
                                         const std::vector<double>& means,
                                         const std::vector<double>& stddevs);
    static double randomMixtureOfExponentials(const std::vector<double>& weights,
                                             const std::vector<double>& lambdas);
    static double randomMixtureOfGammas(const std::vector<double>& weights,
                                        const std::vector<double>& shapes,
                                        const std::vector<double>& scales);
    static double randomMixtureOfBetas(const std::vector<double>& weights,
                                       const std::vector<double>& alphas,
                                       const std::vector<double>& betas);
    static double randomMixtureOfBinomials(int n,
                                           const std::vector<double>& weights,
                                           const std::vector<double>& ps);
    static double randomMixtureOfPoissons(const std::vector<double>& weights,
                                          const std::vector<double>& lambdas);
    static double randomMixtureOfChiSquares(const std::vector<double>& weights,
                                           const std::vector<double>& dfs);
    static double randomMixtureOfFs(const std::vector<double>& weights,
                                    const std::vector<double>& df1s,
                                    const std::vector<double>& df2s);
    static double randomMixtureOfStudentTs(const std::vector<double>& weights,
                                           const std::vector<double>& dfs);
    static double randomMixtureOfBetaPrimes(const std::vector<double>& weights,
                                             const std::vector<double>& alphas,
                                             const std::vector<double>& betas);
    static double randomMixtureOfLaplaces(const std::vector<double>& weights,
                                          const std::vector<double>& mus,
                                          const std::vector<double>& bs);
    static double randomMixtureOfLogistics(const std::vector<double>& weights,
                                           const std::vector<double>& mus,
                                           const std::vector<double>& ss);
    static double randomMixtureOfWeibulls(const std::vector<double>& weights,
                                          const std::vector<double>& shapes,
                                          const std::vector<double>& scales);
    static double randomMixtureOfCauchys(const std::vector<double>& weights,
                                         const std::vector<double>& locations,
                                         const std::vector<double>& scales);
    static double randomMixtureOfDoubleExponentials(const std::vector<double>& weights,
                                                     const std::vector<double>& lambda1s,
                                                     const std::vector<double>& lambda2s);
    
    // Utility functions
    static bool isEven(int n);
    static bool isOdd(int n);
    static bool isPrime(int n);
    static int nextPrime(int n);
    static int previousPrime(int n);
    static std::vector<int> primeFactors(int n);
    static int gcd(int a, int b);
    static int lcm(int a, int b);
    static std::vector<int> factors(int n);
    static std::vector<int> divisors(int n);
    static int digitSum(int n);
    static int digitCount(int n);
    static int reverseDigits(int n);
    static bool isPalindrome(int n);
    static int factorial(int n);
    static int nChooseK(int n, int k);
    static double permutations(int n, int k);
    static double combinations(int n, int k);
    static double permutationsWithRepetitions(int n, const std::vector<int>& repetitions);
    static double combinationsWithRepetitions(int n, int k);
    static double stirlingNumbers(int n, int k);
    static double bernoulliNumber(int n);
    static double eulerNumber(int n);
    static double harmonicNumber(int n);
    static double zeta(int n, double x);
    static double erf(double x);
    static double erfc(double x);
    static double invErf(double x);
    static double invErfc(double x);
    static double gammaFunction(double x);
    static double lnGammaFunction(double x);
    static double digammaFunction(double x);
    static double trigammaFunction(double x);
    static double betaFunction(double x, double y);
    static double lnBetaFunction(double x, double y);
    static double incompleteBeta(double x, double a, double b);
    static double incompleteGamma(double x, double a);
    static double regularizedIncompleteBeta(double x, double a, double b);
    static double regularizedIncompleteGamma(double x, double a);
    static double betainc(double x, double a, double b);
    static double betaincinv(double y, double a, double b);
    static double gammaln(double x);
    static double gammainc(double x, double a, double b = 1.0);
    static double gammaincinv(double p, double a, double b = 1.0);
    static double rbinom(double n, double p);
    static double rnorm(double mean, double sigma);
    static double rexp(double lambda);
    static double rgamma(double shape, double scale);
    static double rbeta(double alpha, double beta);
    static double rchisq(double df);
    static double rf(double df1, double df2);
    static double rt(double df);
    static double rdist(double df1, double df2, double ncp = 0.0);
    static double rhyper(double m, double n, double k);
    static double rgeom(double p);
    static double rpois(double lambda);
    static double rnor(double mu, double sigma);
    static double rlnorm(double mu, double sigma);
    static double rlogis(double location, double scale);
    static double rlogis2(double location, double scale);
    static double rtukey(double df1, double df2, double ncp = 0.0);
    static double rbeta1(double alpha, double beta);
    static double rbeta2(double alpha, double beta);
    static double rbeta3(double alpha, double beta);
    static double rbeta4(double alpha, double beta);
    static double rbeta5(double alpha, double beta);
    static double rbeta6(double alpha, double beta);
    static double rbeta7(double alpha, double beta);
    static double rbeta8(double alpha, double beta);
    static double rbeta9(double alpha, double beta);
    static double rbeta10(double alpha, double beta);
    static double rbeta11(double alpha, double beta);
    static double rbeta12(double alpha, double beta);
    static double rbeta13(double alpha, double beta);
    static double rbeta14(double alpha, double beta);
    static double rbeta15(double alpha, double beta);
    static double rbeta16(double alpha, double beta);
    static double rbeta17(double alpha, double beta);
    static double rbeta18(double alpha, double beta);
    static double rbeta19(double alpha, double beta);
    static double rbeta20(double alpha, double beta);
    static double rbeta21(double alpha, double beta);
    static double rbeta22(double alpha, double beta);
    static double rbeta23(double alpha, double beta);
    static double rbeta24(double alpha, double beta);
    static double rbeta25(double alpha, double beta);
    static double rbeta26(double alpha, double beta);
    static double rbeta27(double alpha, double beta);
    static double rbeta28(double alpha, double beta);
    static double rbeta29(double alpha, double beta);
    static double rbeta30(double alpha, double beta);
    static double rbeta31(double alpha, double beta);
    static double rbeta32(double alpha, double beta);
    static double rbeta33(double alpha, double beta);
    static double rbeta34(double alpha, double beta);
    static double rbeta35(double alpha, double beta);
    static double rbeta36(double alpha, double beta);
    static double rbeta37(double alpha, double beta);
    static double rbeta38(double alpha, double beta);
    static double rbeta39(double alpha, double beta);
    static double rbeta40(double alpha, double beta);
    static double rbeta41(double alpha, double beta);
    static double rbeta42(double alpha, double beta);
    static double rbeta43(double alpha, double beta);
    static double rbeta44(double alpha, double beta);
    static double rbeta45(double alpha, double beta);
    static double rbeta46(double alpha, double beta);
    static double rbeta47(double alpha, double beta);
    static double rbeta48(double alpha, double beta);
    static double rbeta49(double alpha, double beta);
    static double rbeta50(double alpha, double beta);
    static double rbeta51(double alpha, double beta);
    static double rbeta52(double alpha, double beta);
    static double rbeta53(double alpha, double beta);
    static double rbeta54(double alpha, double beta);
    static double rbeta55(double alpha, double beta);
    static double rbeta56(double alpha, double beta);
    static double rbeta57(double alpha, double beta);
    static double rbeta58(double alpha, double beta);
    static double rbeta59(double alpha, double beta);
    static double rbeta60(double alpha, double beta);
    static double rbeta61(double alpha, double beta);
    static double rbeta62(double alpha, double beta);
    static double rbeta63(double alpha, double beta);
    static double rbeta64(double alpha, double beta);
    static double rbeta65(double alpha, double beta);
    static double rbeta66(double alpha, double beta);
    static double rbeta67(double alpha, double beta);
    static double rbeta68(double alpha, double beta);
    static double rbeta69(double alpha, double beta);
    static double rbeta70(double alpha, double beta);
    static double rbeta71(double alpha, double beta);
    static double rbeta72(double alpha, double beta);
    static double rbeta73(double alpha, double beta);
    static double rbeta74(double alpha, double beta);
    static double rbeta75(double alpha, double beta);
    static double rbeta76(double alpha, double beta);
    static double rbeta77(double alpha, double beta);
    static double rbeta78(double alpha, double beta);
    static double rbeta79(double alpha, double beta);
    static double rbeta80(double alpha, double beta);
    static double rbeta81(double alpha, double beta);
    static double rbeta82(double alpha, double beta);
    static double rbeta83(double alpha, double beta);
    static double rbeta84(double alpha, double beta);
    static double rbeta85(double alpha, double beta);
    static double rbeta86(double alpha, double beta);
    static double rbeta87(double alpha, double beta);
    static double rbeta88(double alpha, double beta);
    static double rbeta89(double alpha, double beta);
    static double rbeta90(double alpha, double beta);
    static double rbeta91(double alpha, double beta);
    static double rbeta92(double alpha, double beta);
    static double rbeta93(double alpha, double beta);
    static double rbeta94(double alpha, double beta);
    static double rbeta95(double alpha, double beta);
    static double rbeta96(double alpha, double beta);
    static double rbeta97(double alpha, double beta);
    static double rbeta98(double alpha, double beta);
    static double rbeta99(double alpha, double beta);
    static double rbeta100(double alpha, double beta);
};
}
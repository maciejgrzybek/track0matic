std::unique_ptr<estimation::KalmanFilter<> > kalmanFilter;

estimation::KalmanFilter<>::Matrix A(4,4);
estimation::KalmanFilter<>::Matrix B;
estimation::KalmanFilter<>::Matrix R(2,2);
estimation::KalmanFilter<>::Matrix Q(4,4);
estimation::KalmanFilter<>::Matrix H(2,4);
  
estimation::KalmanFilter<>::Vector X(4);
estimation::KalmanFilter<>::Matrix P(4,4);
  
X.clear();
  
P.clear();
P(0,0) = 0.1;
P(1,1) = 0.1;
P(2,2) = 0.1;
P(3,3) = 0.1;

A.clear();
A(0,0) = 1;
A(0,1) = 0;
A(0,2) = 1;
A(0,3) = 0;

A(1,0) = 0;
A(1,1) = 1;
A(1,2) = 0;
A(1,3) = 1;

A(2,0) = 0;
A(2,1) = 0;
A(2,2) = 1;
A(2,3) = 0;

A(3,0) = 0;
A(3,1) = 0;
A(3,2) = 0;
A(3,3) = 1;

R.clear();
R(0,0) = 100;
R(1,1) = 100;

Q.clear();
Q(0,0) = 0.01;
Q(1,1) = 0.01;
Q(2,2) = 0.01;
Q(3,3) = 0.01;

H.clear();
H(0,0) = 1;
H(0,1) = 0;
H(0,2) = 1;
H(0,3) = 0;
H(1,0) = 0;
H(1,1) = 1;
H(1,2) = 0;
H(1,3) = 1;

estimation::KalmanFilter<>* f = new estimation::KalmanFilter<>(A,B,R,Q,H);
f->initializeState(X,P);
kalmanFilter.reset(f); // f is no more valid!
f = nullptr;

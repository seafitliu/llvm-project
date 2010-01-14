// RUN: %clang_cc1 -fsyntax-only -verify %s

struct X {
  int& f(int) const; // expected-note 2 {{candidate function}}
  float& f(int); // expected-note 2 {{candidate function}}

  void test_f(int x) const {
    int& i = f(x);
  }

  void test_f2(int x) {
    float& f2 = f(x);
  }

  int& g(int) const; // expected-note 2 {{candidate function}}
  float& g(int); // expected-note 2 {{candidate function}}
  static double& g(double); // expected-note 2 {{candidate function}}

  void h(int);

  void test_member() {
    float& f1 = f(0);
    float& f2 = g(0);
    double& d1 = g(0.0);
  }

  void test_member_const() const {
    int &i1 = f(0);
    int &i2 = g(0);
    double& d1 = g(0.0);
  }

  static void test_member_static() {
    double& d1 = g(0.0);
    g(0); // expected-error{{call to 'g' is ambiguous; candidates are:}}
  }
};

void test(X x, const X xc, X* xp, const X* xcp, volatile X xv, volatile X* xvp) {
  int& i1 = xc.f(0);
  int& i2 = xcp->f(0);
  float& f1 = x.f(0);
  float& f2 = xp->f(0);
  xv.f(0); // expected-error{{no matching member function for call to 'f'; candidates are:}}
  xvp->f(0); // expected-error{{no matching member function for call to 'f'; candidates are:}}

  int& i3 = xc.g(0);
  int& i4 = xcp->g(0);
  float& f3 = x.g(0);
  float& f4 = xp->g(0);
  double& d1 = xp->g(0.0);
  double& d2 = X::g(0.0);
  X::g(0); // expected-error{{call to 'g' is ambiguous; candidates are:}}
  
  X::h(0); // expected-error{{call to non-static member function without an object argument}}
}

struct X1 {
  int& member();
  float& member() const;
};

struct X2 : X1 { };

void test_X2(X2 *x2p, const X2 *cx2p) {
  int &ir = x2p->member();
  float &fr = cx2p->member();
}

// Tests the exact text used to note the candidates
namespace test1 {
  class A {
    template <class T> void foo(T t, unsigned N); // expected-note {{candidate function [with T = int] not viable: no known conversion from 'char const [6]' to 'unsigned int' for 2nd argument}}
    void foo(int n, char N); // expected-note {{candidate function not viable: no known conversion from 'char const [6]' to 'char' for 2nd argument}} 
    void foo(int n); // expected-note {{candidate function not viable: requires 1 argument, but 2 were provided}}
    void foo(unsigned n = 10); // expected-note {{candidate function not viable: requires at most 1 argument, but 2 were provided}}
    void foo(int n, const char *s, int t); // expected-note {{candidate function not viable: requires 3 arguments, but 2 were provided}}
    void foo(int n, const char *s, int t, ...); // expected-note {{candidate function not viable: requires at least 3 arguments, but 2 were provided}}
    void foo(int n, const char *s, int t, int u = 0); // expected-note {{candidate function not viable: requires at least 3 arguments, but 2 were provided}}

    void bar(double d); //expected-note {{candidate function not viable: no known conversion from 'class test1::A const' to 'class test1::A' for object argument}}
    void bar(int i); //expected-note {{candidate function not viable: no known conversion from 'class test1::A const' to 'class test1::A' for object argument}}
  };

  void test() {
    A a;
    a.foo(4, "hello"); //expected-error {{no matching member function for call to 'foo'}}

    const A b;
    b.bar(0); //expected-error {{no matching member function for call to 'bar'}}
  }
}


/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2009
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "test/int.hh"

#include <gecode/minimodel.hh>
#include <gecode/scheduling.hh>

namespace Test { namespace Int {

   /// %Tests for unary scheduling constraints
   namespace Unary {

     /**
      * \defgroup TaskTestUnary Unary scheduling constraints
      * \ingroup TaskTestScheduling
      */
     //@{
     /// %Test for unary constraint
     class ManFixUnary : public Test {
     protected:
       /// The processing times
       Gecode::IntArgs p;
       /// Get a reasonable maximal start time
       static int st(const Gecode::IntArgs& p) {
         int t = 0;
         for (int i=p.size(); i--; )
           t += p[i];
         return t;
       }
     public:
       /// Create and register test
       ManFixUnary(const Gecode::IntArgs& p0, int o)
         : Test("Scheduling::Unary::Man::Fix::"+str(o)+"::"+str(p0),
                p0.size(),o,o+st(p0)), 
           p(p0) {
         testsearch = false;
         contest = CTL_NONE;
       }
       /// Create and register initial assignment
       virtual Assignment* assignment(void) const {
         return new RandomAssignment(arity,dom,500);
       }
       /// %Test whether \a x is solution
       virtual bool solution(const Assignment& x) const {
         for (int i=0; i<x.size(); i++)
           for (int j=i+1; j<x.size(); j++)
             if ((x[i]+p[i] > x[j]) && (x[j]+p[j] > x[i]))
               return false;
         return true;
       }
       /// Post constraint on \a x
       virtual void post(Gecode::Space& home, Gecode::IntVarArray& x) {
         Gecode::unary(home, x, p);
       }
     };

     /// %Test for unary constraint with optional tasks
     class OptFixUnary : public Test {
     protected:
       /// The processing times
       Gecode::IntArgs p;
       /// Thereshold for taking a task as optional
       int l;
       /// Get a reasonable maximal start time
       static int st(const Gecode::IntArgs& p) {
         int t = 0;
         for (int i=p.size(); i--; )
           t += p[i];
         return t;
       }
     public:
       /// Create and register test
       OptFixUnary(const Gecode::IntArgs& p0, int o)
         : Test("Scheduling::Unary::Opt::Fix::"+str(o)+"::"+str(p0),
                2*p0.size(),o,o+st(p0)), p(p0), l(o+st(p)/2) {
         testsearch = false;
         contest = CTL_NONE;
       }
       /// Create and register initial assignment
       virtual Assignment* assignment(void) const {
         return new RandomAssignment(arity,dom,500);
       }
       /// %Test whether \a x is solution
       virtual bool solution(const Assignment& x) const {
         int n = x.size() / 2;
         for (int i=0; i<n; i++)
           if (x[n+i] > l)
             for (int j=i+1; j<n; j++)
               if(x[n+j] > l)
                 if ((x[i]+p[i] > x[j]) && (x[j]+p[j] > x[i]))
                   return false;
         return true;
       }
       /// Post constraint on \a x
       virtual void post(Gecode::Space& home, Gecode::IntVarArray& x) {
         int n=x.size() / 2;
         Gecode::IntVarArgs s(n);
         Gecode::BoolVarArgs m(n);
         for (int i=0; i<n; i++) {
           s[i]=x[i];
           m[i]=Gecode::expr(home, (x[n+i] > l));
         }
         Gecode::unary(home, s, p, m);
       }
     };

    /// Generate random selection of assignments
    class RandomMixAssignment : public Assignment {
    protected:
      int* vals; ///< The current values for the variables
      int  a;    ///< How many assigments still to be generated
      int _n1;
      Gecode::IntSet _d1; ///< Domain for second set of variables
      /// Generate new value according to domain \a d
      int randval(const Gecode::IntSet& d) {
        unsigned int skip = Base::rand(d.size());
        for (Gecode::IntSetRanges it(d); true; ++it) {
          if (it.width() > skip)
            return it.min() + static_cast<int>(skip);
          skip -= it.width();
        }
        GECODE_NEVER;
        return 0;
      }
    public:
      /// Initialize for \a a assignments for \a n0 variables and values \a d0
      RandomMixAssignment(int n0, const Gecode::IntSet& d0,
                          int n1, const Gecode::IntSet& d1, int a0)
        : Assignment(n0+n1,d0),vals(new int[n0+n1]),a(a0),_n1(n1),_d1(d1) {
        // std::cerr << "randmix " << _d1 << std::endl;
        for (int i=n0; i--; )
          vals[i] = randval(d);
        for (int i=n1; i--; )
          vals[n0+i] = randval(_d1);
        // for (int i=0; i<n; i++)
        //   std::cerr << vals[i] << " ";
        // std::cerr << std::endl;
      }
      /// Test whether all assignments have been iterated
      virtual bool operator()(void) const {
        return a>0;
      }
      /// Move to next assignment
      virtual void operator++(void) {
        for (int i=n-_n1; i--; )
          vals[i] = randval(d);
        for (int i=_n1; i--; )
          vals[n-_n1+i] = randval(_d1);
        a--;
        // std::cerr << "asn ";
        // for (int i=0; i<n; i++)
        //   std::cerr << vals[i] << " ";
        // std::cerr << std::endl;
      }
      /// Return value for variable \a i
      virtual int operator[](int i) const {
        assert((i>=0) && (i<n));
        return vals[i];
      }
      /// Destructor
      virtual ~RandomMixAssignment(void) {
        delete [] vals;
      }
    };


     /// %Test for unary constraint
     class ManFlexUnary : public Test {
     protected:
       /// Minimum processing time
       int _minP;
       /// Maximum processing time
       int _maxP;
       /// Offset for start times
       int off;
       /// Get a reasonable maximal start time
       static int st(const Gecode::IntArgs& p) {
         int t = 0;
         for (int i=p.size(); i--; )
           t += p[i];
         return t;
       }
     public:
       /// Create and register test
       ManFlexUnary(int n, int minP, int maxP, int o)
         : Test("Scheduling::Unary::Man::Flex::"+str(o)+"::"+str(n)+"::"
                +str(minP)+"::"+str(maxP),
                2*n,0,n*maxP), _minP(minP), _maxP(maxP), off(o) {
         testsearch = false;
         testfix = false;
         contest = CTL_NONE;
       }
       /// Create and register initial assignment
       virtual Assignment* assignment(void) const {
         return new RandomMixAssignment(arity/2,dom,arity/2,
                                        Gecode::IntSet(_minP,_maxP),500);
       }
       /// %Test whether \a x is solution
       virtual bool solution(const Assignment& x) const {
         int n = x.size()/2;
         for (int i=0; i<n; i++)
           for (int j=i+1; j<n; j++)
             if ((x[i]+x[n+i] > x[j]) && (x[j]+x[n+j] > x[i]))
               return false;
         return true;
       }
       /// Post constraint on \a x
       virtual void post(Gecode::Space& home, Gecode::IntVarArray& x) {
         Gecode::IntVarArgs s(x.size()/2);
         Gecode::IntVarArgs px(x.slice(x.size()/2));
         Gecode::IntVarArgs e(home,x.size()/2,
                              Gecode::Int::Limits::min,
                              Gecode::Int::Limits::max);
         for (int i=s.size(); i--;) {
           s[i] = expr(home, off+x[i]);
           rel(home, s[i]+px[i] == e[i]);
           rel(home, _minP <= px[i]);
           rel(home, _maxP >= px[i]);
         }
         Gecode::unary(home, s, px, e);
       }
     };

     /// %Test for unary constraint with optional tasks
     class OptFlexUnary : public Test {
     protected:
       /// Minimum processing time
       int _minP;
       /// Maximum processing time
       int _maxP;
       /// Offset for start times
       int off;
       /// Thereshold for taking a task as optional
       int l;
       /// Get a reasonable maximal start time
       static int st(const Gecode::IntArgs& p) {
         int t = 0;
         for (int i=p.size(); i--; )
           t += p[i];
         return t;
       }
     public:
       /// Create and register test
       OptFlexUnary(int n, int minP, int maxP, int o)
         : Test("Scheduling::Unary::Opt::Flex::"+str(o)+"::"+str(n)+"::"
                +str(minP)+"::"+str(maxP),
                3*n,0,n*maxP), _minP(minP), _maxP(maxP), off(o),
                l(n*maxP/2) {
         testsearch = false;
         testfix = false;
         contest = CTL_NONE;
       }
       /// Create and register initial assignment
       virtual Assignment* assignment(void) const {
         return new RandomMixAssignment(2*(arity/3),dom,arity/3,
                                        Gecode::IntSet(_minP,_maxP),500);
       }
       /// %Test whether \a x is solution
       virtual bool solution(const Assignment& x) const {
         int n = x.size() / 3;
         for (int i=0; i<n; i++)
           if (x[n+i] > l)
             for (int j=i+1; j<n; j++)
               if(x[n+j] > l)
                 if ((x[i]+x[2*n+i] > x[j]) && (x[j]+x[2*n+j] > x[i]))
                   return false;
         return true;
       }
       /// Post constraint on \a x
       virtual void post(Gecode::Space& home, Gecode::IntVarArray& x) {
         int n=x.size() / 3;

         Gecode::IntVarArgs s(n);
         Gecode::IntVarArgs px(n);
         Gecode::IntVarArgs e(home,n,
                              Gecode::Int::Limits::min,
                              Gecode::Int::Limits::max);
         for (int i=n; i--;) {
           s[i] = expr(home, off+x[i]);
           px[i] = x[2*n+i];
           rel(home, s[i]+px[i] == e[i]);
           rel(home, _minP <= px[i]);
           rel(home, _maxP >= px[i]);
         }
         Gecode::BoolVarArgs m(n);
         for (int i=0; i<n; i++)
           m[i]=Gecode::expr(home, (x[n+i] > l));
         Gecode::unary(home, s, px, e, m);
       }
     };

     Gecode::IntArgs p1(4, 2,2,2,2);
     ManFixUnary mfu10(p1,0);
     ManFixUnary mfu1i(p1,Gecode::Int::Limits::min);
     OptFixUnary ofu10(p1,0);
     OptFixUnary ofu1i(p1,Gecode::Int::Limits::min);
     ManFlexUnary mflu10(4,0,2,0);
     ManFlexUnary mflu1i(4,0,2,Gecode::Int::Limits::min);
     ManFlexUnary mflu101(4,1,3,0);
     ManFlexUnary mflu1i1(4,1,3,Gecode::Int::Limits::min);
     OptFlexUnary oflu10(4,0,2,0);
     OptFlexUnary oflu1i(4,0,2,Gecode::Int::Limits::min);

     Gecode::IntArgs p10(5, 2,2,0,2,2);
     ManFixUnary mfu010(p10,0);
     ManFixUnary mfu01i(p10,Gecode::Int::Limits::min);
     OptFixUnary ofu010(p10,0);
     OptFixUnary ofu01i(p10,Gecode::Int::Limits::min);
     ManFlexUnary mflu010(5,0,2,0);
     ManFlexUnary mflu01i(5,0,2,Gecode::Int::Limits::min);
     OptFlexUnary oflu010(5,0,2,0);
     OptFlexUnary oflu01i(5,0,2,Gecode::Int::Limits::min);

     Gecode::IntArgs p2(4, 4,3,3,5);
     ManFixUnary mfu20(p2,0);
     ManFixUnary mfu2i(p2,Gecode::Int::Limits::min);
     OptFixUnary ofu20(p2,0);
     OptFixUnary ofu2i(p2,Gecode::Int::Limits::min);
     ManFlexUnary mflu20(4,3,5,0);
     ManFlexUnary mflu2i(4,3,5,Gecode::Int::Limits::min);
     OptFlexUnary oflu20(4,3,5,0);
     OptFlexUnary oflu2i(4,3,5,Gecode::Int::Limits::min);

     Gecode::IntArgs p20(6, 4,0,3,3,0,5);
     ManFixUnary mfu020(p20,0);
     ManFixUnary mfu02i(p20,Gecode::Int::Limits::min);
     OptFixUnary ofu020(p20,0);
     OptFixUnary ofu02i(p20,Gecode::Int::Limits::min);
     ManFlexUnary mflu020(6,0,5,0);
     ManFlexUnary mflu02i(6,0,5,Gecode::Int::Limits::min);
     OptFlexUnary oflu020(6,0,5,0);
     OptFlexUnary oflu02i(6,0,5,Gecode::Int::Limits::min);

     Gecode::IntArgs p3(6, 4,2,9,3,7,5);
     ManFixUnary mfu30(p3,0);
     ManFixUnary mfu3i(p3,Gecode::Int::Limits::min);
     OptFixUnary ofu30(p3,0);
     OptFixUnary ofu3i(p3,Gecode::Int::Limits::min);
     ManFlexUnary mflu30(6,2,7,0);
     ManFlexUnary mflu3i(6,2,7,Gecode::Int::Limits::min);
     OptFlexUnary oflu30(6,2,7,0);
     OptFlexUnary oflu3i(6,2,7,Gecode::Int::Limits::min);

     Gecode::IntArgs p30(8, 4,0,2,9,3,7,5,0);
     ManFixUnary mfu030(p30,0);
     ManFixUnary mfu03i(p30,Gecode::Int::Limits::min);
     OptFixUnary ofu030(p30,0);
     OptFixUnary ofu03i(p30,Gecode::Int::Limits::min);
     ManFlexUnary mflu030(8,0,9,0);
     ManFlexUnary mflu03i(8,0,9,Gecode::Int::Limits::min);
     OptFlexUnary oflu030(8,0,9,0);
     OptFlexUnary oflu03i(8,0,9,Gecode::Int::Limits::min);

     //@}

   }
}}

// STATISTICS: test-int

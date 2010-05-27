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

#include <gecode/scheduling/unary.hh>

#include <algorithm>

namespace Gecode {

  void
  unary(Home home, const IntVarArgs& s, const IntArgs& p) {
    using namespace Gecode::Scheduling;
    using namespace Gecode::Scheduling::Unary;
    if (s.same(home))
      throw Int::ArgumentSame("Scheduling::unary");
    if (s.size() != p.size())
      throw Int::ArgumentSizeMismatch("Scheduling::unary");
    for (int i=p.size(); i--; ) {
      Int::Limits::nonnegative(p[i],"Scheduling::unary");
      Int::Limits::check(static_cast<double>(s[i].max()) + p[i],
                         "Scheduling::unary");
    }
    if (home.failed()) return;
    TaskArray<ManFixTask> t(home,s.size());
    for (int i=s.size(); i--; )
      t[i].init(s[i],p[i]);
    GECODE_ES_FAIL(ManProp<ManFixTask>::post(home,t));
  }

  void
  unary(Home home, const IntVarArgs& s, const IntArgs& p, 
        const BoolVarArgs& m) {
    using namespace Gecode::Scheduling;
    using namespace Gecode::Scheduling::Unary;
    if (s.same(home))
      throw Int::ArgumentSame("Scheduling::unary");
    if ((s.size() != p.size()) || (s.size() != m.size()))
      throw Int::ArgumentSizeMismatch("Scheduling::unary");
    for (int i=p.size(); i--; ) {
      Int::Limits::nonnegative(p[i],"Scheduling::unary");
      Int::Limits::check(static_cast<double>(s[i].max()) + p[i],
                         "Scheduling::unary");
    }
    if (home.failed()) return;
    TaskArray<OptFixTask> t(home,s.size());
    for (int i=s.size(); i--; )
      t[i].init(s[i],p[i],m[i]);
    GECODE_ES_FAIL(OptProp<OptFixTask>::post(home,t));
  }

  void
  unary(Home home, const IntVarArgs& s, const IntVarArgs& p,
        const IntVarArgs& e) {
    using namespace Gecode::Scheduling;
    using namespace Gecode::Scheduling::Unary;
    if (s.same(home,p+e))
      throw Int::ArgumentSame("Scheduling::unary");
    if ((s.size() != p.size()) || (s.size() != e.size()))
      throw Int::ArgumentSizeMismatch("Scheduling::unary");
    if (home.failed()) return;
    for (int i=p.size(); i--; ) {
      rel(home, p[i], IRT_GQ, 0);
    }
    TaskArray<ManFlexTask> t(home,s.size());
    for (int i=s.size(); i--; )
      t[i].init(s[i],p[i],e[i]);
    GECODE_ES_FAIL(ManProp<ManFlexTask>::post(home,t));
  }

  void
  unary(Home home, const IntVarArgs& s, const IntVarArgs& p, 
        const IntVarArgs& e, const BoolVarArgs& m) {
    using namespace Gecode::Scheduling;
    using namespace Gecode::Scheduling::Unary;
    if (s.same(home,p+e))
      throw Int::ArgumentSame("Scheduling::unary");
    if ((s.size() != p.size()) || (s.size() != m.size()) ||
        (s.size() != e.size()))
      throw Int::ArgumentSizeMismatch("Scheduling::unary");
    if (home.failed()) return;
    for (int i=p.size(); i--; ) {
      rel(home, p[i], IRT_GQ, 0);
    }
    TaskArray<OptFlexTask> t(home,s.size());
    for (int i=s.size(); i--; )
      t[i].init(s[i],p[i],e[i],m[i]);
    GECODE_ES_FAIL(OptProp<OptFlexTask>::post(home,t));
  }

}

// STATISTICS: scheduling-post

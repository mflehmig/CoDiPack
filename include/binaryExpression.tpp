/**
 * CoDiPack, a Code Differentiation Package
 *
 * Copyright (C) 2015 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum, Tim Albring (SciComp, TU Kaiserslautern)
 *
 * This file is part of CoDiPack (http://www.scicomp.uni-kl.de/software/codi).
 *
 * CoDiPack is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * CoDiPack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with CoDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Max Sagebaum, Tim Albring, (SciComp, TU Kaiserslautern)
 *          Prof. Robin Hogan, (Univ. of Reading).
 *
 * Originally based on Adept 1.0 (http://www.met.rdg.ac.uk/clouds/adept/)
 * released under GPL 3.0 (Copyright (C) 2012-2013 Robin Hogan and the University of Reading).
 */

/*
 * In order to include this file the user has to define the preprocessor macro NAME, FUNCTION and PRIMAL_FUNCTION.
 * NAME contains the name of the generated operation. FUNCTION represents the normal name of that function
 * e.g. 'operator -' or 'sin'. PRIMAL_FUNCTION is the name of a function which can call the primal operator.
 *
 * The defines NAME, FUNCTION and PRIMAL_FUNCTION will be undefined at the end of this template.
 *
 * The user needs to define further the following functions:
 *
 * derv11_NAME
 * derv11M_NAME
 * derv10_NAME
 * derv10M_NAME
 * derv01_NAME
 * derv01M_NAME
 */

#ifndef NAME
  #error Please define a name for the binary expression.
#endif
#ifndef FUNCTION
  #error Please define the primal function representation.
#endif
#ifndef PRIMAL_FUNCTION
  #error Please define a function which calls the primal functions representation.
#endif

#define COMBINE2(A,B) A ## B
#define COMBINE(A,B) COMBINE2(A,B)

#define OP NAME
#define OP11 COMBINE(NAME,11)
#define OP10 COMBINE(NAME,10)
#define OP01 COMBINE(NAME,01)
#define FUNC FUNCTION
#define PRIMAL_CALL PRIMAL_FUNCTION
#define DERIVATIVE_FUNC_11   COMBINE(derv11_, NAME)
#define DERIVATIVE_FUNC_11M  COMBINE(derv11M_, NAME)
#define DERIVATIVE_FUNC_10   COMBINE(derv10_, NAME)
#define DERIVATIVE_FUNC_10M  COMBINE(derv10M_, NAME)
#define DERIVATIVE_FUNC_01   COMBINE(derv01_, NAME)
#define DERIVATIVE_FUNC_01M  COMBINE(derv01M_, NAME)

#define GRADIENT_FUNC_A COMBINE(gradientA_, NAME)
#define GRADIENT_FUNC_B COMBINE(gradientB_, NAME)

/* predefine the structs and the functions for higher order derivatives */
template <typename Real, class A, class B> struct OP11;
template <typename Real, class A> struct OP10;
template <typename Real, class B> struct OP01;

template <typename Real, class A, class B>
inline  OP11<Real, A, B> FUNC(const codi::Expression<Real, A>& a, const codi::Expression<Real, B>& b);

template <typename Real, class A>
inline  OP10<Real, A> FUNC(const codi::Expression<Real, A>& a, const typename TypeTraits<Real>::PassiveReal& b);

template <typename Real, class B>
inline  OP01<Real, B> FUNC(const typename TypeTraits<Real>::PassiveReal& a, const codi::Expression<Real, B>& b);

/**
 * @brief Expression implementation for OP with two active variables.
 *
 * @tparam Real The real type used in the active types.
 * @tparam A The expression for the first argument of the function
 * @tparam B The expression for the second argument of the function
 */
template<typename Real, class A, class B>
struct OP11: public Expression<Real, OP11<Real, A, B> > {
  private:
    /** @brief The first argument of the function. */
    const A a_;
    /** @brief The second argument of the function. */
    const B b_;
  public:
    /**
     * @brief The passive type used in the origin.
     *
     * If Real is not an ActiveReal this value corresponds to Real,
     * otherwise the PassiveValue from Real is used.
     */
    typedef typename TypeTraits<Real>::PassiveReal PassiveReal;

    /**
     * @brief Stores both arguments of the expression.
     *
     * @param[in] a  First argument of the expression.
     * @param[in] b  Second argument of the expression.
     */
    OP11(const Expression<Real, A>& a, const Expression<Real, B>& b) :
      a_(a.cast()), b_(b.cast()) {}

    /**
     * @brief Calculates the jacobies of the expression and hands them down to the arguments.
     *
     * For f(x,y) it calculates df/dx and df/dy and passes these values as the multipliers to the arguments.
     *
     * @param[inout] data A helper value which the tape can define and use for the evaluation.
     *
     * @tparam Data The type for the tape data.
     */
    template<typename Data>
    inline void calcGradient(Data& data) const {
      DERIVATIVE_FUNC_11(data, a_, b_, getValue());
    }

    /**
     * @brief Calculates the jacobies of the expression and hands them down to the arguments.
     *
     * For f(x,y) it calculates multiplier * df/dx and multiplier * df/dy and passes these values as the multipliers to the arguments.
     *
     * @param[inout]     data A helper value which the tape can define and use for the evaluation.
     * @param[in]  multiplier The Jacobi from the expression where this expression was used as an argument.
     *
     * @tparam Data The type for the tape data.
     */
    template<typename Data>
    inline void calcGradient(Data& data, const Real& multiplier) const {
      DERIVATIVE_FUNC_11M(data, a_, b_, getValue(), multiplier);
    }

    /**
     * @brief Return the numerical value of the expression.
     *
     * @return The value of the expression.
     */
    inline const Real getValue() const {
      return PRIMAL_CALL(a_.getValue(), b_.getValue());
    }

//    template<typename NewActiveType, typename NewGradientData, size_t activeOffset, size_t passiveOffset>
//    static inline decltype(auto) exchangeActiveType(const Real* primalValues, const NewGradientData* gradientData, const PassiveReal* passiveValues) {
//      return OP11< Real,
//          decltype(A::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset, passiveOffset>(primalValues, gradientData, passiveValues)),
//          decltype(B::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset + ExpressionTraits<A>::maxActiveVariables, passiveOffset + ExpressionTraits<A>::maxPassiveVariables>(primalValues, gradientData, passiveValues))
//          > (
//          A::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset, passiveOffset>(primalValues, gradientData, passiveValues),
//          B::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset + ExpressionTraits<A>::maxActiveVariables, passiveOffset + ExpressionTraits<A>::maxPassiveVariables>(primalValues, gradientData, passiveValues)
//        );
//    }

//    template<typename Data, typename IndexType, typename NewActiveType, typename NewGradientData >
//    static void evalAdjoint2(Data& gradient, const Real& seed, const Real* primalValues, const NewGradientData* gradientData, const PassiveReal* passiveValues) {
//      auto newExpr = exchangeActiveType<NewActiveType, NewGradientData, 0, 0> (primalValues, gradientData, passiveValues);
//      newExpr.calcGradient(gradient, seed);
//    }

    template<typename IndexType, size_t offset, size_t passiveOffset>
    static inline Real getValue(const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues) {
      const Real aPrimal = A::template getValue<IndexType, offset, passiveOffset>(indices, passiveValues, primalValues);
      const Real bPrimal = B::template getValue<IndexType,
          offset + ExpressionTraits<A>::maxActiveVariables,
          passiveOffset + ExpressionTraits<A>::maxPassiveVariables>
            (indices, passiveValues, primalValues);

      return PRIMAL_CALL(aPrimal, bPrimal);
    }

    template<typename IndexType>
    static void evalAdjoint(const Real& seed, const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues, Real* adjointValues) {
      evalAdjointOffset<IndexType, 0, 0>(seed, indices, passiveValues, primalValues, adjointValues);
    }

    template<typename IndexType, size_t offset, size_t passiveOffset>
    static inline void evalAdjointOffset(const Real& seed, const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues, Real* adjointValues) {
      const Real aPrimal = A::template getValue<IndexType, offset, passiveOffset>(indices, passiveValues, primalValues);
      const Real bPrimal = B::template getValue<IndexType,
          offset + ExpressionTraits<A>::maxActiveVariables,
          passiveOffset + ExpressionTraits<A>::maxPassiveVariables>
            (indices, passiveValues, primalValues);
      const Real resPrimal = PRIMAL_CALL(aPrimal, bPrimal);

      const Real aJac = GRADIENT_FUNC_A(aPrimal, bPrimal, resPrimal) * seed;
      const Real bJac = GRADIENT_FUNC_B(aPrimal, bPrimal, resPrimal) * seed;
      A::template evalAdjointOffset<IndexType, offset, passiveOffset>(aJac, indices, passiveValues, primalValues, adjointValues);
      B::template evalAdjointOffset<IndexType,
          offset + ExpressionTraits<A>::maxActiveVariables,
          passiveOffset + ExpressionTraits<A>::maxPassiveVariables>
            (bJac, indices, passiveValues, primalValues, adjointValues);
    }

    inline void pushPassive(const PassiveReal& passive) const {
      a_.pushPassive(passive);
    }
};

/**
 * @brief Expression implementation for OP with one active variables.
 *
 * @tparam Real The real type used in the active types.
 * @tparam A The expression for the first argument of the function
 */
template<typename Real, class A>
struct OP10: public Expression<Real, OP10<Real, A> > {
  public:
    /**
     * @brief The passive type used in the origin.
     *
     * If Real is not an ActiveReal this value corresponds to Real,
     * otherwise the PassiveValue from Real is used.
     */
    typedef typename TypeTraits<Real>::PassiveReal PassiveReal;

  private:
    const A a_;
    const PassiveReal& b_;
  public:

    /**
     * @brief Stores both arguments of the expression.
     *
     * @param[in] a  First argument of the expression.
     * @param[in] b  Second argument of the expression.
     */
    OP10(const Expression<Real, A>& a, const PassiveReal& b) :
      a_(a.cast()), b_(b) {}

    /**
     * @brief Calculates the jacobies of the expression and hands them down to the arguments.
     *
     * For f(x,y) it calculates df/dx passes this value as the multiplier to the argument.
     *
     * @param[inout] data A helper value which the tape can define and use for the evaluation.
     *
     * @tparam Data The type for the tape data.
     */
    template<typename Data>
    inline void calcGradient(Data& data) const {
      DERIVATIVE_FUNC_10(data, a_, b_, getValue());
      a_.pushPassive(b_);
    }

    /**
     * @brief Calculates the jacobies of the expression and hands them down to the arguments.
     *
     * For f(x,y) it calculates multiplier * df/dx and passes this value as the multiplier to the argument.
     *
     * @param[inout]     data A helper value which the tape can define and use for the evaluation.
     * @param[in]  multiplier The Jacobi from the expression where this expression was used as an argument.
     *
     * @tparam Data The type for the tape data.
     */
    template<typename Data>
    inline void calcGradient(Data& data, const Real& multiplier) const {
      DERIVATIVE_FUNC_10M(data, a_, b_, getValue(), multiplier);
      a_.pushPassive(b_);
    }

    /**
     * @brief Return the numerical value of the expression.
     *
     * @return The value of the expression.
     */
    inline const Real getValue() const {
      return PRIMAL_CALL(a_.getValue(), b_);
    }

//    template<typename NewActiveType, typename NewGradientData, size_t activeOffset, size_t passiveOffset>
//    static inline decltype(auto) exchangeActiveType(const Real* primalValues, const NewGradientData* gradientData, const PassiveReal* passiveValues) {
//      return OP10< Real,
//          decltype(A::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset, passiveOffset>(primalValues, gradientData, passiveValues))
//          > (
//          A::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset, passiveOffset>(primalValues, gradientData, passiveValues),
//          passiveValues[passiveOffset + ExpressionTraits<A>::maxPassiveVariables]
//        );
//    }

//    template<typename Data, typename IndexType, typename NewActiveType, typename NewGradientData >
//    static void evalAdjoint2(Data& gradient, const Real& seed, const Real* primalValues, const NewGradientData* gradientData, const PassiveReal* passiveValues) {
//      auto newExpr = exchangeActiveType<NewActiveType, NewGradientData, 0, 0> (primalValues, gradientData, passiveValues);
//      newExpr.calcGradient(gradient, seed);
//    }

    template<typename IndexType, size_t offset, size_t passiveOffset>
    static inline Real getValue(const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues) {
      const Real aPrimal = A::template getValue<IndexType, offset, passiveOffset>(indices, passiveValues, primalValues);
      const PassiveReal& bPrimal = passiveValues[passiveOffset + ExpressionTraits<A>::maxPassiveVariables];

      return PRIMAL_CALL(aPrimal, bPrimal);
    }

    template<typename IndexType>
    static void evalAdjoint(const Real& seed, const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues, Real* adjointValues) {
      evalAdjointOffset<IndexType, 0, 0>(seed, indices, passiveValues, primalValues, adjointValues);
    }

    template<typename IndexType, size_t offset, size_t passiveOffset>
    static inline void evalAdjointOffset(const Real& seed, const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues, Real* adjointValues) {
      const Real aPrimal = A::template getValue<IndexType, offset, passiveOffset>(indices, passiveValues, primalValues);
      const PassiveReal& bPrimal = passiveValues[passiveOffset + ExpressionTraits<A>::maxPassiveVariables];
      const Real resPrimal = PRIMAL_CALL(aPrimal, bPrimal);

      const Real aJac = GRADIENT_FUNC_A(aPrimal, bPrimal, resPrimal) * seed;
      A::template evalAdjointOffset<IndexType, offset, passiveOffset>(aJac, indices, passiveValues, primalValues, adjointValues);
    }

    inline void pushPassive(const PassiveReal& passive) const {
      a_.pushPassive(passive);
    }
};

/**
 * @brief Expression implementation for OP with one active variables.
 *
 * @tparam Real The real type used in the active types.
 * @tparam B The expression for the second argument of the function
 */
template<typename Real, class B>
struct OP01 : public Expression<Real, OP01<Real, B> > {
  public:
    /**
     * @brief The passive type used in the origin.
     *
     * If Real is not an ActiveReal this value corresponds to Real,
     * otherwise the PassiveValue from Real is used.
     */
    typedef typename TypeTraits<Real>::PassiveReal PassiveReal;

  private:
    const PassiveReal& a_;
    const B b_;
  public:

    /**
     * @brief Stores both arguments of the expression.
     *
     * @param[in] a  First argument of the expression.
     * @param[in] b  Second argument of the expression.
     */
    OP01(const PassiveReal& a, const Expression<Real, B>& b) :
      a_(a), b_(b.cast()) {}

    /**
     * @brief Calculates the jacobies of the expression and hands them down to the arguments.
     *
     * For f(x,y) it calculates df/dx passes this value as the multiplier to the argument.
     *
     * @param[inout] data A helper value which the tape can define and use for the evaluation.
     *
     * @tparam Data The type for the tape data.
     */
    template<typename Data>
    inline void calcGradient(Data& data) const {
      b_.pushPassive(a_);
      DERIVATIVE_FUNC_01(data, a_, b_, getValue());
    }

    /**
     * @brief Calculates the jacobies of the expression and hands them down to the arguments.
     *
     * For f(x,y) it calculates multiplier * df/dx and passes this value as the multiplier to the argument.
     *
     * @param[inout]     data A helper value which the tape can define and use for the evaluation.
     * @param[in]  multiplier The Jacobi from the expression where this expression was used as an argument.
     *
     * @tparam Data The type for the tape data.
     */
    template<typename Data>
    inline void calcGradient(Data& data, const Real& multiplier) const {
      b_.pushPassive(a_);
      DERIVATIVE_FUNC_01M(data, a_, b_, getValue(), multiplier);
    }

    /**
     * @brief Return the numerical value of the expression.
     *
     * @return The value of the expression.
     */
    inline const Real getValue() const {
      return PRIMAL_CALL(a_, b_.getValue());
    }

//    template<typename NewActiveType, typename NewGradientData, size_t activeOffset, size_t passiveOffset>
//    static inline decltype(auto) exchangeActiveType(const Real* primalValues, const NewGradientData* gradientData, const PassiveReal* passiveValues) {
//      return OP01< Real,
//          decltype(B::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset, passiveOffset + 1>(primalValues, gradientData, passiveValues))
//          > (
//          passiveValues[passiveOffset],
//          B::template exchangeActiveType<NewActiveType, NewGradientData, activeOffset, passiveOffset + 1>(primalValues, gradientData, passiveValues)
//        );
//    }

//    template<typename Data, typename IndexType, typename NewActiveType, typename NewGradientData >
//    static void evalAdjoint2(Data& gradient, const Real& seed, const Real* primalValues, const NewGradientData* gradientData, const PassiveReal* passiveValues) {
//      auto newExpr = exchangeActiveType<NewActiveType, NewGradientData, 0, 0> (primalValues, gradientData, passiveValues);
//      newExpr.calcGradient(gradient, seed);
//    }

    template<typename IndexType, size_t offset, size_t passiveOffset>
    static inline Real getValue(const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues) {
      const PassiveReal& aPrimal = passiveValues[passiveOffset];
      const Real bPrimal = B::template getValue<IndexType, offset, passiveOffset + 1>(indices, passiveValues, primalValues);

      return PRIMAL_CALL(aPrimal, bPrimal);
    }

    template<typename IndexType>
    static void evalAdjoint(const Real& seed, const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues, Real* adjointValues) {
      evalAdjointOffset<IndexType, 0, 0>(seed, indices, passiveValues, primalValues, adjointValues);
    }

    template<typename IndexType, size_t offset, size_t passiveOffset>
    static inline void evalAdjointOffset(const Real& seed, const IndexType* indices, const PassiveReal* passiveValues, const Real* primalValues, Real* adjointValues) {
      const PassiveReal& aPrimal = passiveValues[passiveOffset];
      const Real bPrimal = B::template getValue<IndexType, offset, passiveOffset + 1>(indices, passiveValues, primalValues);
      const Real resPrimal = PRIMAL_CALL(aPrimal, bPrimal);

      const Real bJac = GRADIENT_FUNC_B(aPrimal, bPrimal, resPrimal) * seed;
      B::template evalAdjointOffset<IndexType, offset, passiveOffset + 1>(bJac, indices, passiveValues, primalValues, adjointValues);
    }

    inline void pushPassive(const PassiveReal& passive) const {
      b_.pushPassive(passive);
    }
};

/**
 * @brief Overload for FUNC with the CoDiPack expressions.
 *
 * @param[in] a  The first argument of the operation.
 * @param[in] b  The second argument of the operation.
 *
 * @return The implementing expression OP.
 *
 * @tparam Real  The real type used in the active types.
 * @tparam    A  The expression for the first argument of the function
 * @tparam    B  The expression for the second argument of the function
 */
template <typename Real, class A, class B>
inline OP11<Real, A, B> FUNC(const codi::Expression<Real, A>& a, const codi::Expression<Real, B>& b) {
  return OP11<Real, A, B>(a.cast(), b.cast());
}
/**
 * @brief Overload for FUNC with the CoDiPack expressions.
 *
 * @param[in] a  The first argument of the operation.
 * @param[in] b  The second argument of the operation.
 *
 * @return The implementing expression OP.
 *
 * @tparam Real  The real type used in the active types.
 * @tparam    A  The expression for the first argument of the function
 */
template <typename Real, class A>
inline OP10<Real, A> FUNC(const codi::Expression<Real, A>& a, const typename TypeTraits<Real>::PassiveReal& b) {
  return OP10<Real, A>(a.cast(), b);
}
/**
 * @brief Overload for FUNC with the CoDiPack expressions.
 *
 * @param[in] a  The first argument of the operation.
 * @param[in] b  The second argument of the operation.
 *
 * @return The implementing expression OP.
 *
 * @tparam Real  The real type used in the active types.
 * @tparam    B  The expression for the second argument of the function
 */
template <typename Real, class B>
inline OP01<Real, B> FUNC(const typename TypeTraits<Real>::PassiveReal& a, const codi::Expression<Real, B>& b) {
  return OP01<Real, B>(a, b.cast());
}

#undef OP
#undef OP11
#undef OP01
#undef OP10
#undef FUNC
#undef PRIMAL_CALL
#undef DERIVATIVE_FUNC_11
#undef DERIVATIVE_FUNC_11M
#undef DERIVATIVE_FUNC_10
#undef DERIVATIVE_FUNC_10M
#undef DERIVATIVE_FUNC_01
#undef DERIVATIVE_FUNC_01M
#undef GRADIENT_FUNC_A
#undef GRADIENT_FUNC_B

#undef COMBINE
#undef COMBINE2

#undef PRIMAL_FUNCTION
#undef FUNCTION
#undef NAME

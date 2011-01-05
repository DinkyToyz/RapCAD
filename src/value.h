/*
 *   RapCAD - Rapid prototyping CAD IDE (www.rapcad.org)
 *   Copyright (C) 2010  Giles Bathgate
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VALUE_H
#define VALUE_H

#include <QString>
#include "expression.h"

class Value
{
public:
	Value();
	void setName(QString);
	QString getName();
	virtual QString getValueString();
	virtual Value* operator*(Value&);
	virtual Value* componentwiseMultiply(Value&);
	virtual Value* operator/(Value&);
	virtual Value* componentwiseDivide(Value&);
	virtual Value* operator%(Value&);
	virtual Value* operator+();
	virtual Value* operator+(Value&);
	virtual Value* operator-();
	virtual Value* operator-(Value&);
	virtual Value* operator<(Value&);
	virtual Value* operator<=(Value&);
	virtual Value* operator==(Value&);
	virtual Value* operator!=(Value&);
	virtual Value* operator>=(Value&);
	virtual Value* operator>(Value&);
	virtual Value* operator&&(Value&);
	virtual Value* operator||(Value&);
	virtual Value* operator!();

	static Value* operation(Value*,Expression::Operator_e);
	static Value* operation(Value*,Expression::Operator_e,Value*);

	template <class A, class B>
	static A basicOperation(B,Expression::Operator_e,B);

	template <class A, class B>
	static A basicOperation(B,Expression::Operator_e);
protected:
	bool isComparison(Expression::Operator_e);
private:
	QString name;

	template<class T>
	static T modulus(T left, T right);
	static double modulus(double left, double right);
};

template<class T>
T Value::modulus(T left, T right)
{
	return left%right;
}

template <class A, class B>
A Value::basicOperation(B left, Expression::Operator_e e, B right)
{
	switch(e) {
	case Expression::Multiply:
		return left*right;
	case Expression::Divide:
		return left/right;
	case Expression::Modulus:
		return modulus(left,right);
	case Expression::Add:
		return left+right;
	case Expression::Subtract:
		return left-right;
	case Expression::LessThan:
		return left<right;
	case Expression::LessOrEqual:
		return left<=right;
	case Expression::Equal:
		return left==right;
	case Expression::NotEqual:
		return left!=right;
	case Expression::GreaterOrEqual:
		return left>=right;
	case Expression::GreaterThan:
		return left>right;
	case Expression::LogicalAnd:
		return left&&right;
	case Expression::LogicalOr:
		return left||right;
	default:
		return left;
	}
}

template <class A, class B>
A Value::basicOperation(B left, Expression::Operator_e e)
{
	switch(e) {
	case Expression::Add:
		return +left;
	case Expression::Subtract:
		return -left;
	case Expression::Invert:
		return !left;
	default:
		return left;
	}
}
#endif // VALUE_H

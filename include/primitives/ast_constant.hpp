#ifndef AST_CONSTANT_HPP
#define AST_CONSTANT_HPP

#include "../ast_node.hpp"
#include "../operations/ast_operand.hpp"

class Constant : public Operand
{
public:
    Type GetType(Context &context) const override = 0;

    /**
     * @brief Save the value of the constant to the global specs
     *
     * This function is used in conjuction with global initialization.
     * A global can only be initialized with a constant value.
     */
    virtual void SaveValue(Global &global_specs) const = 0;

    bool IsPointerOperation(Context &context) const override;
    void EmitRISC(std::ostream &stream, Context &context, std::string passed_reg) const override = 0;
    void Print(std::ostream &stream) const override = 0;
};

class IntConstant : public Constant
{
private:
    int value_;

public:
    IntConstant(int value) : value_(value) {}

    Type GetType(Context &context) const override;
    int GetValue() const;
    void SaveValue(Global &global_specs) const override;

    void EmitRISC(std::ostream &stream, Context &context, std::string passed_reg) const override;
    void Print(std::ostream &stream) const override;
};

class FloatConstant : public Constant
{
private:
    float value_;

public:
    FloatConstant(float value) : value_(value) {}

    Type GetType(Context &context) const override;
    void SaveValue(Global &global_specs) const override;

    void EmitRISC(std::ostream &stream, Context &context, std::string passed_reg) const override;
    void Print(std::ostream &stream) const override;
};

class DoubleConstant : public Constant
{
private:
    double value_;

public:
    DoubleConstant(double value) : value_(value) {}

    Type GetType(Context &context) const override;
    void SaveValue(Global &global_specs) const override;

    void EmitRISC(std::ostream &stream, Context &context, std::string passed_reg) const override;
    void Print(std::ostream &stream) const override;
};

#endif

// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

module Complex
{

//
// The database key (the expression and the result). Naturally, this
// is a stupid key - but this is only a test :)
//
struct Key
{
    string expression;
    int result;
};

//
// A set of classes that represents a numeric parse tree.
//
class Node
{
    int calc();
};

class NumberNode extends Node
{
    int number;
};

class BinaryNode extends Node
{
    Node left;
    Node right;
};

class AddNode extends BinaryNode
{
};

class MultiplyNode extends BinaryNode
{
};

};

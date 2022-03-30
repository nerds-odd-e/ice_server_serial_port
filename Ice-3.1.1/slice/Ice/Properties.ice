// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_PROPERTIES_ICE
#define ICE_PROPERTIES_ICE

#include <Ice/BuiltinSequences.ice>

module Ice
{

/**
 *
 * A simple collection of properties, represented as a dictionary of
 * key/value pairs. Both key and value are [string]s.
 *
 * @see Properties::getPropertiesForPrefix
 *
 **/
local dictionary<string, string> PropertyDict;

/**
 *
 * A property set used to configure &Ice; and &Ice; applications.
 * Properties are key/value pairs, with both keys and values
 * being [string]s. By convention, property keys should have the form
 * <replaceable>application-name</replaceable>\[.<replaceable>category</replaceable>\[.<replaceable>sub-category</replaceable>]].<replaceable>name</replaceable>.
 *
 **/
local interface Properties
{
    /**
     *
     * Get a property by key. If the property does not exist, an empty
     * string is returned.
     *
     * @param key The property key.
     *
     * @return The property value.
     *
     * @see setProperty
     *
     **/
    string getProperty(string key);

    /**
     *
     * Get a property by key. If the property does not exist, the
     * given default value is returned.
     *
     * @param key The property key.
     *
     * @param value The default value to use if the property does not
     * exist.
     *
     * @return The property value or the default value.
     *
     * @see setProperty
     *
     **/
    string getPropertyWithDefault(string key, string value);

    /**
     *
     * Get a property as an integer. If the property does not exist, 0
     * is returned.
     *
     * @param key The property key.
     *
     * @return The property value interpreted as an integer.
     *
     * @see setProperty
     *
     **/
    int getPropertyAsInt(string key);

    /**
     *
     * Get a property as an integer. If the property does not exist, the
     * given default value is returned.
     *
     * @param key The property key.
     *
     * @param value The default value to use if the property does not
     * exist.
     *
     * @return The property value interpreted as an integer, or the
     * default value.
     *
     * @see setProperty
     *
     **/
    int getPropertyAsIntWithDefault(string key, int value);

    /**
     *
     * Get all properties whose keys begins with
     * <replaceable>prefix</replaceable>. If
     * <replaceable>prefix</replaceable> is an empty string,
     * then all properties are returned.
     *
     * @return The matching property set.
     *
     **/
    PropertyDict getPropertiesForPrefix(string prefix);

    /**
     *
     * Set a property. To unset a property, set it to
     * the empty string.
     *
     * @param key The property key.
     * @param value The property value.
     *
     * @see getProperty
     *
     **/
    void setProperty(string key, string value);

    /**
     *
     * Get a sequence of command-line options that is equivalent to
     * this property set. Each element of the returned sequence is
     * a command-line option of the form
     * [--<replaceable>key</replaceable>=<replaceable>value</replaceable>].
     *
     * @return The command line options for this property set.
     *
     **/
    StringSeq getCommandLineOptions();

    /**
     *
     * Convert a sequence of command-line options into properties.
     * All options that begin with
     * [--<replaceable>prefix</replaceable>.] are
     * converted into properties. If the prefix is empty, all options
     * that begin with [--] are converted to properties.
     *
     * @param prefix The property prefix, or an empty string to
     * convert all options starting with [--].
     *
     * @param options The command-line options.
     *
     * @return The command-line options that do not start with the specified
     * prefix, in their original order.
     *
     **/
    StringSeq parseCommandLineOptions(string prefix, StringSeq options);

    /**
     *
     * Convert a sequence of command-line options into properties.
     * All options that begin with one of the following prefixes
     * are converted into properties: [--Ice], [--IceBox], [--IceGrid],
     * [--IcePatch], [--IcePatch2], [--IceSSL], [--IceStorm], [--Freeze],
     * [--Glacier], and [--Glacier2].
     *
     * @param options The command-line options.
     *
     * @return The command-line options that do not start with one of
     * the listed prefixes, in their original order.
     *
     **/
    StringSeq parseIceCommandLineOptions(StringSeq options);

    /**
     *
     * Load properties from a file.
     *
     * @param file The property file.
     *
     **/
    void load(string file);

    /**
     *
     * Create a copy of this property set.
     *
     * @return A copy of this property set.
     *
     **/
    Properties clone();
};

};

#endif

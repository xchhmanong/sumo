/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    LogLevel.java
/// @author  Maximiliano Bottazzi
/// @date    2016
///
//
/****************************************************************************/
package de.dlr.ts.commons.logger;


/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public enum LogLevel { //implements Comparable<LogLevel>
    OFF, SEVERE, WARNING, CONFIG, INFO, FINE, FINER, FINEST, ALL;

    /**
     *
     * @param level
     * @return
     */
    public static LogLevel parse(String level) {
        try {
            Integer i = Integer.valueOf(level);

            if (i > 7) {
                return ALL;
            }

            return LogLevel.values()[i];
        } catch (Exception e) {
            for (LogLevel ll : LogLevel.values())
                if (ll.toString().equalsIgnoreCase(level)) {
                    return ll;
                }
        }

        return null;
    }


}

/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEDetectorE2.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDetectorE2.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane* lane, GNENet* net, double pos, double length, const std::string &freq, const std::string &trafficLight, const std::string& filename, 
                             const std::string& vehicleTypes, const std::string& name, SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) :
    GNEDetector(id, net, GLO_E2DETECTOR, SUMO_TAG_E2DETECTOR, pos, freq, filename, vehicleTypes, name, friendlyPos, blockMovement, {lane}),
    myLength(length),
    myTrafficLight(trafficLight),
    myEndPositionOverLane(0.),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myE2valid(true) {
}


GNEDetectorE2::GNEDetectorE2(const std::string& id, std::vector<GNELane*> lanes, GNENet* net, double pos, double endPos, const std::string &freq, const std::string &trafficLight, const std::string& filename, 
                             const std::string& vehicleTypes, const std::string& name, SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) :
    GNEDetector(id, net, GLO_E2DETECTOR, SUMO_TAG_E2DETECTOR_MULTILANE, pos, freq, filename, vehicleTypes, name, friendlyPos, blockMovement, lanes),
    myLength(0),
    myTrafficLight(trafficLight),
    myEndPositionOverLane(endPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myE2valid(true) {
}


GNEDetectorE2::~GNEDetectorE2() {
}


bool
GNEDetectorE2::isAdditionalValid() const {
    if (getParentLanes().size() == 1) {
        // with friendly position enabled position are "always fixed"
        if (myFriendlyPosition) {
            return true;
        } else {
            return (myPositionOverLane >= 0) && ((myPositionOverLane + myLength) <= getParentLanes().front()->getParentEdges().front()->getNBEdge()->getFinalLength());
        }
    } else {
        // first check if there is connection between all consecutive lanes
        if (myE2valid) {
            // with friendly position enabled position are "always fixed"
            if (myFriendlyPosition) {
                return true;
            } else {
                return (myPositionOverLane >= 0) && ((myPositionOverLane) <= getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength() &&
                                                     myEndPositionOverLane >= 0) && ((myEndPositionOverLane) <= getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength());
            }
        } else {
            return false;
        }
    }
}


std::string
GNEDetectorE2::getAdditionalProblem() const {
    // declare variable for error position
    std::string errorFirstLanePosition, separator, errorLastLanePosition;
    if (getParentLanes().size() == 1) {
        // check positions over lane
        if (myPositionOverLane < 0) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPositionOverLane > getParentLanes().front()->getParentEdges().front()->getNBEdge()->getFinalLength()) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
        }
        if ((myPositionOverLane + myLength) > getParentLanes().front()->getParentEdges().front()->getNBEdge()->getFinalLength()) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " + " + toString(SUMO_ATTR_LENGTH) + " > lanes's length");
        }
    } else {
        if (myE2valid) {
            // check positions over first lane
            if (myPositionOverLane < 0) {
                errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " < 0");
            }
            if (myPositionOverLane > getParentLanes().front()->getParentEdges().front()->getNBEdge()->getFinalLength()) {
                errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
            }
            // check positions over last lane
            if (myEndPositionOverLane < 0) {
                errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " < 0");
            }
            if (myEndPositionOverLane > getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength()) {
                errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
            }
        } else {
            errorFirstLanePosition = "lanes aren't consecutives";
        }
    }
    // check separator
    if ((errorFirstLanePosition.size() > 0) && (errorLastLanePosition.size() > 0)) {
        separator = " and ";
    }
    // return error message
    return errorFirstLanePosition + separator + errorLastLanePosition;
}


void
GNEDetectorE2::fixAdditionalProblem() {
    if (getParentLanes().size() == 1) {
        // obtain position and length
        double newPositionOverLane = myPositionOverLane;
        double newLength = myLength;
        // fix pos and length using fixE2DetectorPosition
        GNEAdditionalHandler::fixE2DetectorPosition(newPositionOverLane, newLength, getParentLanes().at(0)->getParentEdges().front()->getNBEdge()->getFinalLength(), true);
        // set new position and length
        setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_LENGTH, toString(myLength), myNet->getViewNet()->getUndoList());
    } else {
        if (!myE2valid) {
            // build connections between all consecutive lanes
            bool foundConnection = true;
            int i = 0;
            // iterate over all lanes, and stop if myE2valid is false
            while (i < ((int)getParentLanes().size() - 1)) {
                // change foundConnection to false
                foundConnection = false;
                // if a connection betwen "from" lane and "to" lane of connection is found, change myE2valid to true again
                for (auto j : getParentLanes().at(i)->getParentEdges().front()->getGNEConnections()) {
                    if (j->getLaneFrom() == getParentLanes().at(i) && j->getLaneTo() == getParentLanes().at(i + 1)) {
                        foundConnection = true;
                    }
                }
                // if connection wasn't found
                if (!foundConnection) {
                    // create new connection manually
                    NBEdge::Connection newCon(getParentLanes().at(i)->getIndex(), getParentLanes().at(i + 1)->getParentEdges().front()->getNBEdge(), getParentLanes().at(i + 1)->getIndex());
                    // allow to undo creation of new lane
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Connection(getParentLanes().at(i)->getParentEdges().front(), newCon, false, true), true);
                }
                // update lane iterator
                i++;
            }
        } else {
            // declare new position
            double newPositionOverLane = myPositionOverLane;
            // fix pos and length  checkAndFixDetectorPosition
            GNEAdditionalHandler::checkAndFixDetectorPosition(newPositionOverLane, getParentLanes().front()->getParentEdges().front()->getNBEdge()->getFinalLength(), true);
            // set new position
            setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
            // declare new end position
            double newEndPositionOverLane = myEndPositionOverLane;
            // fix pos and length  checkAndFixDetectorPosition
            GNEAdditionalHandler::checkAndFixDetectorPosition(newEndPositionOverLane, getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength(), true);
            // set new position
            setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPositionOverLane), myNet->getViewNet()->getUndoList());
        }
    }
}


void
GNEDetectorE2::moveGeometry(const Position& offset) {
    // Calculate new position using old position
    Position newPosition = myMove.originalViewPosition;
    newPosition.add(offset);
    // filtern position using snap to active grid
    newPosition = myNet->getViewNet()->snapToActiveGrid(newPosition);
    double offsetLane = getParentLanes().front()->getLaneShape().nearest_offset_to_point2D(newPosition, false) - getParentLanes().front()->getLaneShape().nearest_offset_to_point2D(myMove.originalViewPosition, false);
    // move geometry depending of number of lanes
    if (getParentLanes().size() == 1) {
        // calculate new position over lane
        double newPositionOverLane = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
        // obtain lane length
        double laneLength = getParentLanes().front()->getParentEdges().front()->getNBEdge()->getFinalLength() * getLane()->getLengthGeometryFactor();
        if (newPositionOverLane < 0) {
            myPositionOverLane = 0;
        } else if (newPositionOverLane + myLength > laneLength) {
            myPositionOverLane = laneLength - myLength;
        } else {
            myPositionOverLane = newPositionOverLane;
        }
    } else {
        // calculate new start and end positions
        double newStartPosition = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
        double newEndPosition = parse<double>(myMove.secondOriginalPosition) + offsetLane;
        // change start and end position of E2 detector ONLY if both extremes aren't overpassed
        if ((newStartPosition >= 0) && (newStartPosition <= getParentLanes().front()->getLaneShapeLength()) &&
                (newEndPosition >= 0) && (newEndPosition <= getParentLanes().back()->getLaneShapeLength())) {
            myPositionOverLane = newStartPosition;
            myEndPositionOverLane = newEndPosition;
        }
    }
    // Update geometry
    updateGeometry();
}


void
GNEDetectorE2::commitGeometryMoving(GNEUndoList* undoList) {
    // commit geometry moving depending of number of lanes
    if (getParentLanes().size() == 1) {
        // commit new position allowing undo/redo
        undoList->p_begin("position of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPositionOverLane), myMove.firstOriginalLanePosition));
        undoList->p_end();
    } else {
        undoList->p_begin("position of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPositionOverLane), myMove.firstOriginalLanePosition));
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(myEndPositionOverLane), myMove.secondOriginalPosition));
        undoList->p_end();
    }
}


void
GNEDetectorE2::updateGeometry() {
    // declare variables for start and end positions
    double startPosFixed, endPosFixed;
    // set start position
    if (myPositionOverLane < 0) {
        startPosFixed = 0;
    } else if (myPositionOverLane > getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength()) {
        startPosFixed = getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength();
    } else {
        startPosFixed = myPositionOverLane;
    }
    // set end position
    if ((myPositionOverLane + myLength) < 0) {
        endPosFixed = 0;
    } else if ((myPositionOverLane + myLength) > getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength()) {
        endPosFixed = getParentLanes().back()->getParentEdges().front()->getNBEdge()->getFinalLength();
    } else {
        endPosFixed = (myPositionOverLane + myLength);
    }
    if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
        // declare extreme geometry
        GNEGeometry::ExtremeGeometry extremeGeometry;
        // set extremes
        extremeGeometry.laneStartPosition = startPosFixed;
        extremeGeometry.laneEndPosition = endPosFixed;
        // calculate consecutive path using parent lanes
        calculateConsecutivePathLanes(getParentLanes());
        // calculate edge geometry path using path
        GNEGeometry::calculateLaneGeometricPath(myAdditionalSegmentGeometry, getPath(), extremeGeometry);
    } else {
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), startPosFixed * getParentLanes().front()->getLengthGeometryFactor(), endPosFixed * getParentLanes().back()->getLengthGeometryFactor());
        // update block icon position
        myBlockIcon.updatePositionAndRotation();
        // Set offset of the block icon
        myBlockIcon.setOffset(1, 0);
    }
}


double
GNEDetectorE2::getLength() const {
    return myLength;
}


void
GNEDetectorE2::checkE2MultilaneIntegrity() {
    // we assume that E2 is valid
    myE2valid = true;
    int i = 0;
    // iterate over all lanes, and stop if myE2valid is false
    while (i < ((int)getParentLanes().size() - 1) && myE2valid) {
        // set myE2valid to false
        myE2valid = false;
        // if a connection betwen "from" lane and "to" lane of connection is found, change myE2valid to true again
        for (auto j : getParentLanes().at(i)->getParentEdges().front()->getGNEConnections()) {
            if (j->getLaneFrom() == getParentLanes().at(i) && j->getLaneTo() == getParentLanes().at(i + 1)) {
                myE2valid = true;
            }
        }
        // update iterator
        i++;
    }
}


void
GNEDetectorE2::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double E2Exaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if ((myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) && s.drawAdditionals(E2Exaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // declare color
        RGBColor E2Color, textColor;
        // set color
        if (drawUsingSelectColor()) {
            E2Color = s.colorSettings.selectedAdditionalColor;
            textColor = E2Color.changedBrightness(-32);
        } else if (myE2valid) {
            E2Color = s.detectorSettings.E2Color;
            textColor = RGBColor::BLACK;
        }
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // push layer matrix
        glPushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E2DETECTOR);
        // set color
        GLHelper::setColor(E2Color);
        // draw geometry
        GNEGeometry::drawGeometry(myNet->getViewNet(), myAdditionalGeometry, s.detectorSettings.E2Width * E2Exaggeration);
        // Check if the distance is enought to draw details
        if (s.drawDetail(s.detailSettings.detectorDetails, E2Exaggeration)) {
            // draw E2 Logo
            drawDetectorLogo(s, E2Exaggeration, "E2", textColor);
            // Show Lock icon depending of the Edit mode
            myBlockIcon.drawIcon(s, E2Exaggeration);
        }
        // pop layer matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        drawName(getPositionInView(), s.scale, s.addName);
        // Pop name
        glPopName();
        // draw additional name
        drawAdditionalName(s);
        // check if dotted contours has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->getInspectedAttributeCarrier() == this) {
            GNEGeometry::drawDottedContourShape(true, s, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width, E2Exaggeration);
        }
        if (s.drawDottedContour() || myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GNEGeometry::drawDottedContourShape(false, s, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width, E2Exaggeration);
        }
    }
}


std::string
GNEDetectorE2::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            return parseIDs(getParentLanes());
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPositionOverLane);
        case SUMO_ATTR_FREQUENCY:
            return myFreq;
        case SUMO_ATTR_TLID:
            return myTrafficLight;
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return myVehicleTypes;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_TLID:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_LANE:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNELane*> >(myNet, value, false);
            }
        case SUMO_ATTR_LANES:
            if (value.empty()) {
                return false;
            } else if (canParse<std::vector<GNELane*> >(myNet, value, false)) {
                // check if lanes are consecutives
                return lanesConsecutives(parse<std::vector<GNELane*> >(myNet, value));
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_FREQUENCY:
            return value.empty() || (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_TLID:
            /* temporal */
            return true;
        case SUMO_ATTR_LENGTH:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE2::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            checkE2MultilaneIntegrity();
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = value;
            break;
        case SUMO_ATTR_TLID:
            myTrafficLight = value;
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = value;
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/

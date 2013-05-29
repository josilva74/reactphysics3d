/********************************************************************************
* ReactPhysics3D physics library, http://code.google.com/p/reactphysics3d/      *
* Copyright (c) 2010-2013 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

#ifndef REACTPHYSICS3D_SLIDER_JOINT_H
#define REACTPHYSICS3D_SLIDER_JOINT_H

// Libraries
#include "../mathematics/mathematics.h"
#include "../engine/ConstraintSolver.h"

namespace reactphysics3d {

// Structure SliderJointInfo
/**
 * This structure is used to gather the information needed to create a slider
 * joint. This structure will be used to create the actual slider joint.
 */
struct SliderJointInfo : public ConstraintInfo {

    public :

        // -------------------- Attributes -------------------- //

        /// Anchor point (in world-space coordinates)
        Vector3 anchorPointWorldSpace;

        /// Slider axis (in world-space coordinates)
        Vector3 sliderAxisWorldSpace;

        /// True if the slider limits are enabled
        bool isLimitEnabled;

        /// True if the slider motor is enabled
        bool isMotorEnabled;

        /// Lower limit
        decimal lowerLimit;

        /// Upper limit
        decimal upperLimit;

        /// Motor speed
        decimal motorSpeed;

        /// Maximum motor force (in Newton) that can be applied to reach to desired motor speed
        decimal maxMotorForce;

        /// Constructor without limits and without motor
        SliderJointInfo(RigidBody* rigidBody1, RigidBody* rigidBody2,
                        const Vector3& initAnchorPointWorldSpace,
                        const Vector3& initSliderAxisWorldSpace)
                       : ConstraintInfo(rigidBody1, rigidBody2, SLIDERJOINT),
                         anchorPointWorldSpace(initAnchorPointWorldSpace),
                         sliderAxisWorldSpace(initSliderAxisWorldSpace),
                         isLimitEnabled(false), isMotorEnabled(false), lowerLimit(-1.0),
                         upperLimit(1.0), motorSpeed(0), maxMotorForce(0) {}

        /// Constructor with limits and no motor
        SliderJointInfo(RigidBody* rigidBody1, RigidBody* rigidBody2,
                        const Vector3& initAnchorPointWorldSpace,
                        const Vector3& initSliderAxisWorldSpace,
                        decimal initLowerLimit, decimal initUpperLimit)
                       : ConstraintInfo(rigidBody1, rigidBody2, SLIDERJOINT),
                         anchorPointWorldSpace(initAnchorPointWorldSpace),
                         sliderAxisWorldSpace(initSliderAxisWorldSpace),
                         isLimitEnabled(true), isMotorEnabled(false), lowerLimit(initLowerLimit),
                         upperLimit(initUpperLimit), motorSpeed(0), maxMotorForce(0) {}

        /// Constructor with limits and motor
        SliderJointInfo(RigidBody* rigidBody1, RigidBody* rigidBody2,
                        const Vector3& initAnchorPointWorldSpace,
                        const Vector3& initSliderAxisWorldSpace,
                        decimal initLowerLimit, decimal initUpperLimit,
                        decimal initMotorSpeed, decimal initMaxMotorForce)
                       : ConstraintInfo(rigidBody1, rigidBody2, SLIDERJOINT),
                         anchorPointWorldSpace(initAnchorPointWorldSpace),
                         sliderAxisWorldSpace(initSliderAxisWorldSpace),
                         isLimitEnabled(true), isMotorEnabled(true), lowerLimit(initLowerLimit),
                         upperLimit(initUpperLimit), motorSpeed(initMotorSpeed),
                         maxMotorForce(initMaxMotorForce) {}
};

// Class SliderJoint
/**
 * This class represents a slider joint.
 */
class SliderJoint : public Constraint {

    private :

        // -------------------- Constants -------------------- //

        // Beta value for the position correction bias factor
        static const decimal BETA;

        // -------------------- Attributes -------------------- //

        /// Anchor point of body 1 (in local-space coordinates of body 1)
        Vector3 mLocalAnchorPointBody1;

        /// Anchor point of body 2 (in local-space coordinates of body 2)
        Vector3 mLocalAnchorPointBody2;

        /// Slider axis (in local-space coordinates of body 1)
        Vector3 mSliderAxisBody1;

        /// Initial orientation difference between the two bodies
        Quaternion mInitOrientationDifference;

        /// First vector orthogonal to the slider axis local-space of body 1
        Vector3 mN1;

        /// Second vector orthogonal to the slider axis and mN1 in local-space of body 1
        Vector3 mN2;

        /// Vector r1 in world-space coordinates
        Vector3 mR1;

        /// Vector r2 in world-space coordinates
        Vector3 mR2;

        /// Cross product of r2 and n1
        Vector3 mR2CrossN1;

        /// Cross product of r2 and n2
        Vector3 mR2CrossN2;

        /// Cross product of r2 and the slider axis
        Vector3 mR2CrossSliderAxis;

        /// Cross product of vector (r1 + u) and n1
        Vector3 mR1PlusUCrossN1;

        /// Cross product of vector (r1 + u) and n2
        Vector3 mR1PlusUCrossN2;

        /// Cross product of vector (r1 + u) and the slider axis
        Vector3 mR1PlusUCrossSliderAxis;

        /// Bias of the 2 translation constraints
        Vector2 mBTranslation;

        /// Bias of the 3 rotation constraints
        Vector3 mBRotation;

        /// Bias of the lower limit constraint
        decimal mBLowerLimit;

        /// Bias of the upper limit constraint
        decimal mBUpperLimit;

        /// Inverse of mass matrix K=JM^-1J^t for the translation constraint (2x2 matrix)
        Matrix2x2 mInverseMassMatrixTranslationConstraint;

        /// Inverse of mass matrix K=JM^-1J^t for the rotation constraint (3x3 matrix)
        Matrix3x3 mInverseMassMatrixRotationConstraint;

        /// Inverse of mass matrix K=JM^-1J^t for the upper and lower limit constraints (1x1 matrix)
        decimal mInverseMassMatrixLimit;

        /// Inverse of mass matrix K=JM^-1J^t for the motor
        decimal mInverseMassMatrixMotor;

        /// Impulse for the 2 translation constraints
        Vector2 mImpulseTranslation;

        /// Impulse for the 3 rotation constraints
        Vector3 mImpulseRotation;

        /// Impulse for the lower limit constraint
        decimal mImpulseLowerLimit;

        /// Impulse for the upper limit constraint
        decimal mImpulseUpperLimit;

        /// Impulse for the motor
        decimal mImpulseMotor;

        /// True if the slider limits are enabled
        bool mIsLimitEnabled;

        /// True if the motor of the joint in enabled
        bool mIsMotorEnabled;

        /// Slider axis in world-space coordinates
        Vector3 mSliderAxisWorld;

        /// Lower limit
        decimal mLowerLimit;

        /// Upper limit
        decimal mUpperLimit;

        /// True if the lower limit is violated
        bool mIsLowerLimitViolated;

        /// True if the upper limit is violated
        bool mIsUpperLimitViolated;

        /// Motor speed
        decimal mMotorSpeed;

        /// Maximum motor force (in Newton) that can be applied to reach to desired motor speed
        decimal mMaxMotorForce;

        // -------------------- Methods -------------------- //

        /// Reset the limits
        void resetLimits();

    public :

        // -------------------- Methods -------------------- //

        /// Constructor
        SliderJoint(const SliderJointInfo& jointInfo);

        /// Destructor
        virtual ~SliderJoint();

        /// Return true if the limits or the joint are enabled
        bool isLimitEnabled() const;

        /// Return true if the motor of the joint is enabled
        bool isMotorEnabled() const;

        /// Enable/Disable the limits of the joint
        void enableLimit(bool isLimitEnabled);

        /// Enable/Disable the motor of the joint
        void enableMotor(bool isMotorEnabled);

        /// Return the lower limit
        decimal getLowerLimit() const;

        /// Set the lower limit
        void setLowerLimit(decimal lowerLimit);

        /// Return the upper limit
        decimal getUpperLimit() const;

        /// Set the upper limit
        void setUpperLimit(decimal upperLimit);

        /// Return the motor speed
        decimal getMotorSpeed() const;

        /// Set the motor speed
        void setMotorSpeed(decimal motorSpeed);

        /// Return the maximum motor force
        decimal getMaxMotorForce() const;

        /// Set the maximum motor force
        void setMaxMotorForce(decimal maxMotorForce);

        /// Return the intensity of the current force applied for the joint motor
        decimal getMotorForce(decimal timeStep) const;

        /// Return the number of bytes used by the joint
        virtual size_t getSizeInBytes() const;

        /// Initialize before solving the constraint
        virtual void initBeforeSolve(const ConstraintSolverData& constraintSolverData);

        /// Warm start the constraint (apply the previous impulse at the beginning of the step)
        virtual void warmstart(const ConstraintSolverData& constraintSolverData);

        /// Solve the velocity constraint
        virtual void solveVelocityConstraint(const ConstraintSolverData& constraintSolverData);

        /// Solve the position constraint
        virtual void solvePositionConstraint(const ConstraintSolverData& constraintSolverData);
};

// Return true if the limits or the joint are enabled
inline bool SliderJoint::isLimitEnabled() const {
    return mIsLimitEnabled;
}

// Return true if the motor of the joint is enabled
inline bool SliderJoint::isMotorEnabled() const {
    return mIsMotorEnabled;
}

// Return the lower limit
inline decimal SliderJoint::getLowerLimit() const {
    return mLowerLimit;
}

// Return the upper limit
inline decimal SliderJoint::getUpperLimit() const {
    return mUpperLimit;
}

// Return the motor speed
inline decimal SliderJoint::getMotorSpeed() const {
    return mMotorSpeed;
}

// Return the maximum motor force
inline decimal SliderJoint::getMaxMotorForce() const {
    return mMaxMotorForce;
}

// Return the intensity of the current force applied for the joint motor
inline decimal SliderJoint::getMotorForce(decimal timeStep) const {
    return mImpulseMotor / timeStep;
}

// Return the number of bytes used by the joint
inline size_t SliderJoint::getSizeInBytes() const {
    return sizeof(SliderJoint);
}

}

#endif

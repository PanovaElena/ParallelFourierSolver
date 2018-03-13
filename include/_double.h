#pragma once
struct Double {
    double d;
    
    Double() { d = 0; }

    Double Conjugate() const {
        return d;
    }

    Double(double _d) { d = _d; }
    operator double() const { return d; }
};
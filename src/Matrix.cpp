#include "Matrix.h"
#include <cmath>
using namespace std;
namespace fre {
    Vector operator*(const Matrix& C, const Vector& V)
    {
        int d = (int)C.size();
        Vector W(d);
        for (int j = 0; j < d; j++)
        {
            W[j] = 0.0;
            for (int l = 0; l < d; l++) W[j] = W[j] + C[j][l] * V[l];
        }
        return W;
    }

    Vector operator+(const Vector& V, const Vector& W)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = V[j] + W[j];
        return U;
    }
    
    Vector operator-(const Vector& V, const Vector& W)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = V[j] - W[j];
        return U;
    }
    
    Vector operator/(const Vector& V, const Vector& W)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = V[j] / W[j];
        return U;
    }

    Vector operator+(const double& a, const Vector& V)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = a + V[j];
        return U;
    }

    Vector operator*(const double& a, const Vector& V)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = a * V[j];
        return U;
    }

    Vector operator*(const Vector& V, const Vector& W)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = V[j] * W[j];
        return U;
    }

    Vector exp(const Vector& V)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = std::exp(V[j]);
        return U;
    }

    double operator^(const Vector& V, const Vector& W)
    {
        double sum = 0.0;
        int d = (int)V.size();
        for (int j = 0; j < d; j++) sum = sum + V[j] * W[j];
        return sum;
    }

    // overload cout for vector, cout every element in the vector
    ostream& operator<<(ostream& out, Vector& V)
    {
        int num_elements = V.size();
        int num_columns = ceil(sqrt(num_elements));
        int num_rows = ceil(static_cast<double>(num_elements) / num_columns);
    
        for (int i = 0; i < num_columns; ++i) {
            for (int j = 0; j < num_rows; ++j) {
                int index = j * num_columns + i;
                if (index < num_elements) {
                    cout << fixed<<setprecision(6)<< V[index] <<  "\t";
                }
            }
            cout<<endl;
            
        }
        
        
        return out;
    }
    
        ostream& operator<<(ostream& out, vector<string>& V)
    {
        int num_elements = V.size();
        int num_columns = ceil(sqrt(num_elements));
        int num_rows = ceil(static_cast<double>(num_elements) / num_columns);
    
        for (int i = 0; i < num_columns; ++i) {
            for (int j = 0; j < num_rows; ++j) {
                int index = j * num_columns + i;
                if (index < num_elements) {
                    cout << fixed<<setprecision(6)<< V[index] <<  "\t";
                }
            }
            cout<<endl;
            
        }
        
        
        return out;
    }

    ostream& operator<<(ostream& out, Matrix& W)
    {
        // printing the column names for the calculations.
        int counter = 0;
        vector<string> columnNames = {"AAR Mean", "CAR Mean", "AAR Std Dev", "CAR Std Dev" };
        
        for (Matrix::iterator itr = W.begin(); itr != W.end(); itr++){
            cout<< columnNames[counter]<<endl;
            out << *itr<< endl;    // Use ostream & operator<<(ostream & out, Vector & V)
            counter+=1;
        }
        out << "----------------------------------------------------------------------------" <<endl;
        return out;
    }
}



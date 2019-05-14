/*****************************************************************************
 Implementation of Fast Fourier Transformation on Finite Elements
 *****************************************************************************
 * @author     Marius van der Wijden
 * Copyright [2019] [Marius van der Wijden]
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#define DEBUG

#include "device_field.h"
#include "device_field_operators.h"
#include <assert.h>
#include <gmp.h>
#include <iostream>

namespace fields{

    void testAdd()
    {
        printf("Addition test: ");
        fields::Field f1(1234);
        fields::Field f2(1234);
        fields::Field result(2468);
        add(f1,f2);
        testEquality(f1, result);
        printf("successful\n");
    }

    void testSubstract()
    {
        printf("Substraction test: ");
        fields::Field f1(1234);
        fields::Field f2(1234);
        substract(f1,f2);
        testEquality(f1, fields::Field::zero());
        fields::Field f3(1235);
        substract(f3, f2);
        testEquality(f3,fields::Field::one());
        printf("successful\n");
    }

    void testMultiply()
    {
        printf("Multiply test: ");
        fields::Field f1(1234);
        fields::Field f2(1234);
        mul(f1, f2);
        testEquality(f1, fields::Field(1522756));
        fields::Field f3(1234);
        square(f3);
        testEquality(f1, f3);
        printf("successful\n");
    }

    void testModulo()   
    {
        printf("Modulo test: ");
        fields::Field f1(uint32_t(0));
        fields::Field f2(1234);
        
        fields::Field f3();
        printf("successful\n");
    }

    void testPow()
    {
        printf("POW test: ");
        fields::Field f1(2);
        pow(f1, 0);
        testEquality(f1, fields::Field::one());
        fields::Field f2(2);
        pow(f2, 2);
        testEquality(f2, fields::Field(4));
        pow(f2, 10);
        testEquality(f2, fields::Field(1048576));
        fields::Field f3(2);
        fields::Field f4(1048576);
        pow(f3, 20);
        testEquality(f3, f4);
        printf("successful\n");

    }

    void testConstructor()
    {
        printf("Constructor test: ");
        fields::Field f3(1);
        testEquality(f3, fields::Field::one());
        fields::Field f4;
        testEquality(f4, fields::Field::zero());
        fields::Field f5(uint32_t(0));
        testEquality(f5, fields::Field::zero());

        fields::Field f1;
        fields::Field f2(1234);
        add(f1, fields::Field(1234));
        testEquality(f1, f2);
        uint32_t tmp [SIZE] ={0,0,0,0,0,0,0,1234};
        fields::Field f6(tmp);
        testEquality(f6, f2);
        printf("successful\n");
    }

    void setMod()
    {
        assert(SIZE == 8);
        _mod[0] = 0;
        _mod[1] = 0;
        _mod[2] = 0;
        _mod[3] = 0;
        _mod[4] = 0;
        _mod[5] = 0;
        _mod[6] = 1;
        _mod[7] = 0;
    }

    void operate(fields::Field & f1, fields::Field const f2, int const op)
    {
        switch(op){
            case 0:
                add(f1,f2); break;
            case 1:
                substract(f1,f2); break;
            case 2:
                mul(f1,f2); break;
            case 3:
                pow(f1,f2.im_rep[SIZE - 1]); break;
            default: break;
        } 
    }

    void operate(mpz_t mpz1, mpz_t const mpz2, mpz_t const mod, int const op)
    {
        switch(op){
            case 0:
                mpz_add(mpz1, mpz1, mpz2);
                mpz_mod(mpz1, mpz1, mod);
                break;
            case 1:
                mpz_sub(mpz1, mpz1, mpz2);
                mpz_mod(mpz1, mpz1, mod); 
                break;
            case 2:
                mpz_mul(mpz1, mpz1, mpz2);
                mpz_mod(mpz1, mpz1, mod);
                break;
            case 3:
                mpz_t pow;
                mpz_init(pow);
                mpz_set_ui(pow, 4294967295);
                mpz_and(pow, mpz2, pow);
                mpz_powm(mpz1, mpz1, pow, mod);
                break;
            default: break;
        }
    }

    void toMPZ(mpz_t ret, fields::Field f)
    {
        mpz_init(ret);
        mpz_import(ret, SIZE, 1, sizeof(uint32_t), 0, 0, f.im_rep);   
    }

    void compare(fields::Field f1, fields::Field f2, mpz_t mpz1, mpz_t mpz2, mpz_t mod, int op)
    {
        printField(f1);
        printField(f2);
        gmp_printf ("[%Zd] : [%Zd] : %d\n", mpz1, mpz2, op);
        operate(f1, f2, op);
        operate(mpz1, mpz2, mod, op);
        mpz_t tmp;
        toMPZ(tmp, f1);
        
        gmp_printf ("[%Zd] : [%Zd] \n", mpz1, tmp);
        assert(mpz_cmp(tmp, mpz1) == 0);
    }

    void fuzzTest()
    {
        mpz_t a, b, mod;
        mpz_init(a);
        mpz_init(b);
        mpz_init(mod);
        mpz_set_ui(mod, 4294967296);
        for(uint i = 0; i < 4294967295; i++)
        {
            for(uint k = 0; k < 4294967295; k++)
            {
                for(uint z = 0; z <= 3; z++ )
                {
                    mpz_set_ui(a, i);
                    mpz_set_ui(b, k);
                    fields::Field f1(i);
                    fields::Field f2(k);
                    compare(f1,f2,a,b,mod,z);
                }
            }
        }
        
        
    }
}

int main(int argc, char** argv)
{
    fields::setMod();
    fields::testConstructor();
    fields::testAdd();
    fields::testSubstract();
    fields::testMultiply();
    fields::testPow();
    fields::fuzzTest();
    printf("\nAll tests successful\n");
    return 0;
}




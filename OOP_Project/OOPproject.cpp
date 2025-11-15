#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

//     BASE CLASS : Candidate

class Candidate {
protected:
    string id, name, email, type;
    int age;
    vector<string> skills;

public:
    Candidate(string id, string name, string email, int age, string type, vector<string> skills)
        : id(id), name(name), email(email), age(age), type(type), skills(skills) {}

    virtual double calculateScore(const vector<string>& reqSkills) const = 0;

    string getType() const { return type; }

    virtual string toCSV(double score) const {
        stringstream ss;
        ss << id << "," << name << "," << email << "," << age << "," << type << ",";

        for (int i = 0; i < skills.size(); i++) {
            ss << skills[i];
            if (i < skills.size() - 1) ss << ";";
        }

        ss << "," << score;
        return ss.str();
    }

    virtual ~Candidate() {}
};

//   DERIVED CLASS : Fresher

class Fresher : public Candidate {
    double cgpa;

public:
    Fresher(string id, string name, string email, int age, string type,
            vector<string> skills, double cgpa)
        : Candidate(id, name, email, age, type, skills), cgpa(cgpa) {}

    double calculateScore(const vector<string>& reqSkills) const override {
        return cgpa;
    }

    string toCSV(double score) const override {
        return Candidate::toCSV(score) + "," + to_string(cgpa);
    }
};

//    DERIVED CLASS : Experienced

class Experienced : public Candidate {
    double years;

public:
    Experienced(string id, string name, string email, int age, string type,
                vector<string> skills, double years)
        : Candidate(id, name, email, age, type, skills), years(years) {}

    double calculateScore(const vector<string>& reqSkills) const override {
        return years;
    }

    string toCSV(double score) const override {
        return Candidate::toCSV(score) + "," + to_string(years);
    }
};

//  CLASS : RecruitmentSystem

class RecruitmentSystem {
    vector<Candidate*> candidates;

public:

    void loadCSV(const string& file) {
        ifstream fin(file);
        string line;

        getline(fin, line); // skip header

        while (getline(fin, line)) {
            stringstream ss(line);
            string id, name, email, ageS, type, skillsStr, field;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, email, ',');
            getline(ss, ageS, ',');
            getline(ss, type, ',');
            getline(ss, skillsStr, ',');

            vector<string> skills;
            string temp;
            stringstream sk(skillsStr);
            while (getline(sk, temp, ';')) {
                skills.push_back(temp);
            }

            int age = stoi(ageS);

            if (type == "Fresher") {
                getline(ss, field, ',');
                double cgpa = stod(field);
                candidates.push_back(new Fresher(id, name, email, age, type, skills, cgpa));
            }
            else {
                getline(ss, field, ',');
                double exp = stod(field);
                candidates.push_back(new Experienced(id, name, email, age, type, skills, exp));
            }
        }
    }

    void writeCSV(const string& filename, const vector<pair<double, Candidate*>>& list) {
        ofstream fout(filename);
        fout << "ID,Name,Email,Age,Type,Skills,Score,ExtraField\n";

        for (auto& p : list) {
            fout << p.second->toCSV(p.first) << "\n";
        }
        fout.close();
    }

    void shortlist(int fresherReq, int expReq) {
        vector<pair<double, Candidate*>> freshers;
        vector<pair<double, Candidate*>> experienced;

        vector<string> reqSkills = {"C++", "Python"};

        for (auto c : candidates) {
            double score = c->calculateScore(reqSkills);
            if (c->getType() == "Fresher")
                freshers.push_back({score, c});
            else
                experienced.push_back({score, c});
        }

        sort(freshers.rbegin(), freshers.rend());
        sort(experienced.rbegin(), experienced.rend());

        vector<pair<double, Candidate*>> topFreshers(
            freshers.begin(), freshers.begin() + min(fresherReq, (int)freshers.size())
        );

        vector<pair<double, Candidate*>> topExperienced(
            experienced.begin(), experienced.begin() + min(expReq, (int)experienced.size())
        );

        cout << "\n--- Shortlisted Freshers ---\n";
        for (auto& p : topFreshers)
            cout << p.second->toCSV(p.first) << endl;

        cout << "\n--- Shortlisted Experienced ---\n";
        for (auto& p : topExperienced)
            cout << p.second->toCSV(p.first) << endl;

        writeCSV("shortlisted_freshers.csv", topFreshers);
        writeCSV("shortlisted_experienced.csv", topExperienced);

        cout << "\nCSV files generated successfully.\n";
    }

    ~RecruitmentSystem() {
        for (auto c : candidates) {
            delete c;
        }
    }
};

// MAIN function

int main() {
    RecruitmentSystem rs;
    rs.loadCSV("candidates.csv");

    int fresherCount, experiencedCount;

    cout << "Enter number of freshers to shortlist: ";
    cin >> fresherCount;

    cout << "Enter number of experienced candidates to shortlist: ";
    cin >> experiencedCount;

    rs.shortlist(fresherCount, experiencedCount);

    return 0;
}

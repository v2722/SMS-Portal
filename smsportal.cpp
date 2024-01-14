#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <algorithm>
#include <memory>
#include <set>
#include <cctype> // Include for handling whitespace

using namespace std;

struct SMS {
    string from;
    string to;
    string emailid;
    string message_details;
    string date;

    bool operator==(const SMS& other) const {
        return (from == other.from) && (to == other.to) &&
               (message_details == other.message_details) && (date == other.date);
    }

    bool operator<(const SMS& other) const {
        return date < other.date;
    }
};

class LinkedList {
public:
    struct Node {
        SMS data;
        unique_ptr<Node> next;

        Node(const SMS& sms) : data(sms), next(nullptr) {}
    };

private:
    unique_ptr<Node> head;

public:
    LinkedList() : head(nullptr) {}

    void pb(const SMS& sms) {
        if (!head) {
            head = make_unique<Node>(sms);
        } else {
            Node* current = head.get();
            while (current->next) {
                current = current->next.get();
            }
            current->next = make_unique<Node>(sms);
        }
    }

    void display() {
        Node* current = head.get();
        while (current) {
            cout << "From: " << current->data.from << endl;
            cout << "To: " << current->data.to << endl;
            cout << "Email-id: " << current->data.emailid << endl;
            cout << "Message: " << current->data.message_details << endl;
            cout << "Date: " << current->data.date << endl;
            current = current->next.get();
        }
    }

    bool deleteSMS(const SMS& sms) {
        if (!head) {
            return false;
        }

        if (head->data == sms) {
            head = move(head->next);
            return true;
        }

        Node* current = head.get();
        while (current->next) {
            if (current->next->data == sms) {
                current->next = move(current->next->next);
                return true;
            }
            current = current->next.get();
        }

        return false;
    }
};

map<string, LinkedList> userMessages;
map<string, vector<SMS>> dateMessages;
map<string, vector<SMS>> searchIndex;

void addSMS(const SMS& sms) {
    LinkedList& messageList = userMessages[sms.to];
    messageList.pb(sms);
    dateMessages[sms.to].push_back(sms);
    dateMessages[sms.from].push_back(sms);
    searchIndex[sms.from].push_back(sms);
    searchIndex[sms.to].push_back(sms);
    searchIndex[sms.emailid].push_back(sms); // Add the SMS to the email-id index
}

void searchMessagesByEmail(const string& email) {
    if (searchIndex.find(email) == searchIndex.end()) {
        cout << "Email not found. No messages found." << endl;
        return;
    }

    set<SMS> foundMessages;
    for (const SMS& message : searchIndex[email]) {
        cout << "From: " << message.from << endl;
        cout << "To: " << message.to << endl;
        cout << "Email-id: " << message.emailid << endl;
        cout << "Message: " << message.message_details << endl;
        cout << "Date: " << message.date << endl;
        cout << endl;
    }
}

void deleteAllMessagesForEmail(const string& email) {
    if (searchIndex.find(email) == searchIndex.end()) {
        cout << "Email not found. No messages deleted." << endl;
        return;
    }

    for (auto it = searchIndex[email].begin(); it != searchIndex[email].end();) {
        SMS& message = *it;
        userMessages[message.to].deleteSMS(message);
        dateMessages[message.to].erase(
            remove(dateMessages[message.to].begin(), dateMessages[message.to].end(), message),
            dateMessages[message.to].end()
        );

        dateMessages[message.from].erase(
            remove(dateMessages[message.from].begin(), dateMessages[message.from].end(), message),
            dateMessages[message.from].end()
        );
        it = searchIndex[email].erase(it);
    }

    cout << "All messages associated with email " << email << " have been deleted." << endl;
}

struct Conversation {
    string participant1;
    string participant2;
    vector<SMS> messages;

    bool operator==(const Conversation& other) const {
        return (participant1 == other.participant1) && (participant2 == other.participant2);
    }
};

class ConversationManager {
    vector<Conversation> conversations;

public:
    void addConversation(const Conversation& conversation) {
        conversations.push_back(conversation);
    }

    void displayConversation(const string& participant1, const string& participant2) {
        Conversation searchConversation;
        searchConversation.participant1 = participant1;
        searchConversation.participant2 = participant2;

        auto it = find(conversations.begin(), conversations.end(), searchConversation);

        if (it != conversations.end()) {
            cout << "Conversation between " << participant1 << " and " << participant2 << ":\n";
            for (const SMS& sms : it->messages) {
                cout << "Message: " << sms.message_details << endl;
                cout << "Date: " << sms.date << endl;
                cout << endl;
            }
        } else {
            cout << "No conversation found." << endl;
        }
    }
};

int main() {
    ConversationManager conversationManager;
    bool exitRequested = false;

    while (!exitRequested) {
        cout << "SMS Portal Menu:" << endl;
        cout << "1. Add SMS" << endl;
        cout << "2. Search Messages by Email" << endl;
        cout << "3. Delete SMS" << endl;
        cout << "4. Add Conversation" << endl;
        cout << "5. Display Conversation" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore(); // Consume the newline character

        switch (choice) {
            case 1: {
                SMS sms;
                cout << "Enter From: ";
                getline(cin, sms.from);
                cout << "Enter To: ";
                getline(cin, sms.to);
                cout << "Enter Message: ";
                getline(cin, sms.message_details);
                cout << "Enter Email-id: ";
                getline(cin, sms.emailid);

                // Adjusting time based on your requirements
                time_t now = time(0);
                tm* gmtm = gmtime(&now);
                gmtm->tm_hour += 5;
                gmtm->tm_min += 30;
                mktime(gmtm);

                char dt[30];
                strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", gmtm);
                cout << dt << endl;
                sms.date = dt;

                addSMS(sms);
                cout << "SMS added successfully!" << endl;
                break;
            }
            case 2: {
                cout << "Enter the email-id: ";
                string searchStr;
                getline(cin, searchStr);
                searchMessagesByEmail(searchStr);
                break;
            }
            case 3: {
                string emailToDelete;
                cout << "Enter the email-id to delete all associated messages: ";
                cin >> emailToDelete;
                deleteAllMessagesForEmail(emailToDelete);
                break;
            }
            case 4: {
                Conversation conversation;
                SMS sms;
                cout << "Enter Participant 1: ";
                getline(cin, conversation.participant1);
                cout << "Enter Participant 2: ";
                getline(cin, conversation.participant2);

                int numMessages;
                cout << "Enter the number of messages: ";
                cin >> numMessages;
                cin.ignore(); // Consume the newline character

                for (int i = 0; i < numMessages; i++) {
                    cout << "Enter Message " << i + 1 << ": ";
                    getline(cin, sms.message_details);

                    // Adjusting time based on your requirements
                    time_t now = time(0);
                    tm* gmtm = gmtime(&now);
                    gmtm->tm_hour += 5;
                    gmtm->tm_min += 30;
                    mktime(gmtm);

                    char dt[30];
                    strftime(dt, sizeof(dt), "%Y-%m-%d %H:%M:%S", gmtm);

                    sms.date = dt;
                    conversation.messages.push_back(sms);
                }
                conversationManager.addConversation(conversation);
                cout << "Conversation added successfully!" << endl;
                break;
            }
            case 5: {
                string participant1, participant2;
                cout << "Enter Participant 1: ";
                getline(cin, participant1);
                cout << "Enter Participant 2: ";
                getline(cin, participant2);

                conversationManager.displayConversation(participant1, participant2);
                break;
            }
            case 6:
                exitRequested = true;
                cout << "Exiting the SMS Portal. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
        }
    }

    return 0;
}
from flask import Flask, request, jsonify
from joblib import load

app = Flask(__name__)
model = load('"C:\Users\ASUS\OneDrive\Documents\AltairRapidMiner\AI Studio\Repositories\Final Year Project\processor\Downloads\model.pkl"')

@app.route('/predict', methods=['POST'])  # Only POST allowed
def predict():
    gas = request.json['gas']
    status, day = model.predict([[gas]])[0]
    return jsonify({"status": status, "day": day})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
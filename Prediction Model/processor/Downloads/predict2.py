import sys
import joblib
import warnings
warnings.filterwarnings("ignore")


model = joblib.load(r"C:\Users\ASUS\OneDrive\Documents\AltairRapidMiner\AI Studio\Repositories\Final Year Project\processor\Downloads\model2.pkl")
gas = float(sys.argv[1])
prediction = model.predict([[gas, gas]])
status = prediction[0]
print(status)
print(gas)

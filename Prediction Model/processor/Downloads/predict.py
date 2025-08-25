import sys
import joblib
import warnings
warnings.filterwarnings("ignore")


# Load model (make sure path is correct)
model = joblib.load(r"C:\Users\ASUS\OneDrive\Documents\AltairRapidMiner\AI Studio\Repositories\Final Year Project\processor\Downloads\model2.pkl")

# Read gas value from command line
gas = float(sys.argv[1])

# Predict status
prediction = model.predict([[gas, gas]])  # if model expects 2 features, put gas twice or fix model input

# Just for example, replace with your actual logic:
status = prediction[0]

# Print output in a clear format (this will be captured by Node-RED exec)
print(status)
print(gas)


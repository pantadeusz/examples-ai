import streamlit as st
import requests
import json


st.title("LLM Chat")
query = st.text_area("Enter your message:",key='prompt_text_area')
if st.button("Send") and query:
	with st.empty():  # Placeholder for streaming response
		try:
			response = requests.post(
				"http://localhost:11434/api/generate",
				json={
					"model": "wizardlm2:latest",  # Replace with the specific model you want to use
					"prompt": query,
					"temperature": 0.7,  # Adjust as needed
					"max_tokens": 200,  # Set token limit
					"stream": True
				},
				stream=True
			)
			if response.status_code == 200:
				i = 0
				result = ""
				for line in response.iter_lines(decode_unicode=True):
					if line.strip():
						try:
							data = json.loads(line)
							if "response" in data:
								result += data["response"]
								i = i + 1
								st.text_area("Response:", result, height=200, key='dynamic_response_'+str(i))
								
						except json.JSONDecodeError:
							# Handle any malformed lines gracefully
							st.text_area("Response:", f"Error decoding JSON: {line}", height=200, key='response_error')
			else:
				st.text_area("Response:", f"Error: {response.status_code} - {response.text}", height=200)
		except Exception as e:
			st.text_area("Response:", f"An error occurred: {e}", height=200, key='response_error')
			
